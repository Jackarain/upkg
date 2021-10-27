﻿#include "upkg/upkg.hpp"

#include <QDebug>
#include <QCryptographicHash>

#include "upkg/qcommondelegate.hpp"
#include "upkg/url_parser.hpp"

#ifdef _MSC_VER
#	include <windows.h>
#	pragma comment(lib, "Version")
#endif

extern QFont* global_default_font;

QString GetFileVertion(QString fullName)
{
	QString result = "0.0.0.0";
#ifdef WIN32
	DWORD dwlen = GetFileVersionInfoSizeA(fullName.toStdString().c_str(), 0);

	if (0 == dwlen)
		return result;

	std::string data;
	data.resize(dwlen + 1);
	BOOL bSuccess = GetFileVersionInfoA(fullName.toStdString().c_str(), 0, dwlen, data.data());

	if (false == bSuccess)
		return result;

	LPVOID lpBuffer = nullptr;
	UINT uLen = 0;
	struct LANGANDCODEPAGE
	{
		WORD wLanguage;
		WORD wCodePage;
	}* lpTranslate;

	bSuccess = VerQueryValue(data.data(), (TEXT("\\VarFileInfo\\Translation")), (LPVOID*)&lpTranslate, &uLen);
	if (false == bSuccess)
		return result;

	QString str1, str2;
	str1.setNum(lpTranslate->wLanguage, 16);
	str2.setNum(lpTranslate->wCodePage, 16);
	str1 = "000" + str1;
	str2 = "000" + str2;
	QString verPath = "\\StringFileInfo\\" + str1.right(4) + str2.right(4) + "\\FileVersion";
	bSuccess = VerQueryValueA(data.data(), (verPath.toStdString().c_str()), &lpBuffer, &uLen);
	if (false == bSuccess)
		return result;

	result = QString::fromLocal8Bit((char*)lpBuffer);
#endif
	return result;
}

upkg::upkg(QWidget *parent)
	: QMainWindow(parent)
	, m_settings(QSettings::NativeFormat, QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName())
{
	m_ui.setupUi(this);

	m_datamodel = new Datamodel(m_ui.fileListView);
	m_ui.fileListView->setModel(m_datamodel);

	m_ui.fileListView->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_ui.fileListView->setSelectionMode(QAbstractItemView::SingleSelection);
	m_ui.fileListView->horizontalHeader()->setDisabled(false);

	m_ui.fileListView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
	m_ui.fileListView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
	m_ui.fileListView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Interactive);
	m_ui.fileListView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Interactive);
	m_ui.fileListView->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Interactive);
	m_ui.fileListView->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Interactive);
	m_ui.fileListView->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Interactive);
	m_ui.fileListView->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Interactive);

	QFontMetrics fm(*global_default_font);
	auto columnWidth = fm.horizontalAdvance(QString("b0baee9d279d34fa1dfd71aadb908c3f")) + 7;	// 7 column line width.
	m_ui.fileListView->setColumnWidth(2, columnWidth);
	m_ui.fileListView->setColumnWidth(3, columnWidth);

	m_ui.fileListView->horizontalHeader()->setStretchLastSection(true);
	m_ui.fileListView->setSortingEnabled(true);

	m_ui.fileListView->show();
	m_ui.fileListView->setItemDelegate(new QCommonDelegate(m_ui.fileListView));

	m_ui.InputDirEdit->setReadOnly(true);
	QObject::connect(m_ui.InputDirBtn, &QPushButton::clicked, [this]() mutable
	{
		QString path = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, tr("请选择源目录"), QDir::currentPath()));
		if (path.isEmpty())
			return;
		auto oriPath = m_ui.InputDirEdit->text();
		if (oriPath != path)
		{
			if (m_scanning_thrd.isRunning())
			{
				m_abort = true;
				m_scanning_thrd.waitForFinished();
				assert(!m_abort && "waitForFinished");
			}

			m_datamodel->deleteAllData();
			m_ui.InputDirEdit->setText(path);

			loadDir();
		}
	});

	m_ui.OutputDirEdit->setReadOnly(true);
	QObject::connect(m_ui.OutputDirBtn, &QPushButton::clicked, [this]() mutable
	{
		QString path = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, tr("请选择存放目录"), QDir::currentPath()));
		if (path.isEmpty())
			return;
		if (QDir(path).entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries).count() != 0)
		{
			QMessageBox::information(this, tr("目录不为空"), tr("目录不为空，请使用一个空目录用来保存生成的更新文件!"), QMessageBox::Yes);
			return;
		}
		m_ui.OutputDirEdit->setText(path);
	});

	QObject::connect(m_ui.startBtn, &QPushButton::clicked, [this]() mutable
	{
		auto inputDir = m_ui.InputDirEdit->text();
		auto outputDir = m_ui.OutputDirEdit->text();
		auto urlPath = m_ui.UrlEdit->text();
		auto xmlFileName = m_ui.XmlEdit->text();

		if (inputDir.isEmpty() || outputDir.isEmpty() || urlPath.isEmpty() || xmlFileName.isEmpty())
		{
			QMessageBox::information(this, tr("参数缺失"), tr("参数设置错误, 请检查参数设置!"), QMessageBox::Yes);
			return;
		}

		if (m_scanning_thrd.isRunning())
		{
			QMessageBox::warning(this, tr("正在扫描目录"), tr("正在扫描目录, 请先停止或等待扫描完成后再重试!"), QMessageBox::Yes);
			return;
		}

		try
		{
			util::uri url{ urlPath.toStdString() };
		}
		catch (const std::exception&)
		{
			QMessageBox::warning(this, tr("URL格式错误"), tr("URL格式错误, 请使用正确的Url再重试!"), QMessageBox::Yes);
			return;
		}


	});

	QObject::connect(m_ui.stopBtn, &QPushButton::clicked, [this]() mutable
	{
		m_abort = true;
	});

	QObject::connect(this, &upkg::workDir, [this](const QDir& dir) mutable
	{
		walkDir(dir);
		m_abort = false;
	});

	QObject::connect(this, &upkg::scanDir, this, [this]() mutable
	{
		loadSettings();
		loadDir();
	}, Qt::QueuedConnection);

	Q_EMIT this->scanDir();
}

upkg::~upkg()
{
	m_abort = true;
	m_scanning_thrd.waitForFinished();
	saveSettings();
}

void upkg::loadSettings() noexcept
{
	auto inputDir = m_settings.value("InputDir").toString();
	if (!inputDir.isEmpty())
		m_ui.InputDirEdit->setText(inputDir);
	auto outputDir = m_settings.value("OutputDir").toString();
	if (!outputDir.isEmpty())
		m_ui.OutputDirEdit->setText(outputDir);
	auto urlPath = m_settings.value("Url").toString();
	if (!urlPath.isEmpty())
		m_ui.UrlEdit->setText(urlPath);
	auto xmlFileName = m_settings.value("Xml").toString();
	if (!xmlFileName.isEmpty())
		m_ui.XmlEdit->setText(xmlFileName);
	if (m_settings.contains("WinSize"))
		resize(m_settings.value("WinSize").toSize());
	if (m_settings.contains("Geometry"))
		restoreGeometry(m_settings.value("Geometry").toByteArray());
	if (m_settings.contains("State"))
		restoreState(m_settings.value("State").toByteArray());

	for (int i = 0; i < m_ui.fileListView->horizontalHeader()->count(); ++i)
	{
		auto columnName = "Columns" + QString::number(i);
		if (m_settings.contains(columnName))
		{
			auto column = m_settings.value(columnName).toInt();
			m_ui.fileListView->setColumnWidth(i, column);
		}
	}
}

void upkg::saveSettings() noexcept
{
	auto inputDir = m_ui.InputDirEdit->text();
	auto outputDir = m_ui.OutputDirEdit->text();
	auto urlPath = m_ui.UrlEdit->text();
	auto xmlFileName = m_ui.XmlEdit->text();

	for (int i = 0; i < m_ui.fileListView->horizontalHeader()->count(); ++i)
	{
		auto column = m_ui.fileListView->columnWidth(i);
		m_settings.setValue("Columns" + QString::number(i), column);
	}

	m_settings.setValue("InputDir", inputDir);
	m_settings.setValue("OutputDir", outputDir);
	m_settings.setValue("Url", urlPath);
	m_settings.setValue("Xml", xmlFileName);
	m_settings.setValue("WinSize", size());
	m_settings.setValue("State", saveState());
	m_settings.setValue("Geometry", saveGeometry());
}

void upkg::loadDir() noexcept
{
	auto inputDir = m_ui.InputDirEdit->text();
	if (inputDir.isEmpty())
		return;

	if (m_scanning_thrd.isRunning())
	{
		QMessageBox::warning(this, tr("正在扫描目录"), tr("正在扫描目录, 请先停止或等待扫描完成后再重试!"), QMessageBox::Yes);
		return;
	}

	m_scanning_thrd = QtConcurrent::run([this, inputDir] { workDir(inputDir); });
}

QFileInfoList upkg::walkDir(const QDir& dir)
{
	QFileInfoList fileList = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	QFileInfoList folderList = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
	const auto readBufferSize = 512 * 1024;
	static QByteArray buffer(readBufferSize, 0);

	for (auto& fileinfo : fileList)
	{
		if (m_abort)
			break;

		auto fileName = fileinfo.fileName();
		auto zipFileName = fileName + ".zip";
		auto fileSize = fileinfo.size();

		ModelData data;

		data.m_fileversion = GetFileVertion(fileinfo.absoluteFilePath());
		data.m_filesize = QString::number(fileSize);
		data.m_filename = fileName;
		data.m_filepath = fileinfo.absoluteFilePath();
		data.m_file_type = tr("使用zip方式");

		QFile infile(fileinfo.absoluteFilePath());
		if (infile.open(QIODevice::ReadOnly))
		{
			QCryptographicHash hash(QCryptographicHash::Md5);
			if (!infile.atEnd())
			{
				for (; !m_abort;)
				{
					auto readBytes = infile.read(buffer.data(), readBufferSize);
					if (readBytes == 0)
						break;
					buffer.resize(readBytes);
					hash.addData(buffer);
				}

				data.m_md5 = hash.result().toHex();
			}
		}

		m_datamodel->insertData(data);
	}

	for (auto& folder : folderList)
	{
		if (m_abort)
			break;
		walkDir(folder.absoluteFilePath());
	}

	return fileList;
}
