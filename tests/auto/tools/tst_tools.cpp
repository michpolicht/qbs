/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Build Suite.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#include <tools/logger.h>
#include <tools/fileinfo.h>
#include <tools/options.h>
#include <QDir>
#include <QtTest>

class TestTools : public QObject
{
    Q_OBJECT
private slots:
    void testValidCommandLine()
    {
        QStringList args;
        args.append("-vvk");
        args.append("-v");
        args << "--products" << "blubb";
        args << "--changed-files" << "foo,bar";
        qbs::CommandLineOptions options;
        QVERIFY(options.parseCommandLine(args));
        QCOMPARE(qbs::Logger::instance().level(), qbs::LoggerTrace);
        QCOMPARE(options.command(), qbs::CommandLineOptions::BuildCommand);
        QCOMPARE(options.buildOptions().selectedProductNames, QStringList() << "blubb");
        QCOMPARE(options.buildOptions().changedFiles.count(), 2);
        QVERIFY(options.buildOptions().keepGoing);
        QVERIFY(options.parseCommandLine(QStringList() << "-vvvqqq"));
        QCOMPARE(qbs::Logger::instance().level(), qbs::Logger::defaultLevel());
        QVERIFY(options.parseCommandLine(QStringList() << "-vvqqq"));
        QCOMPARE(qbs::Logger::instance().level(), qbs::LoggerWarning);
        QVERIFY(options.parseCommandLine(QStringList() << "-vvvqq"));
        QCOMPARE(qbs::Logger::instance().level(), qbs::LoggerDebug);
        QVERIFY(options.parseCommandLine(QStringList() << "--log-level" << "trace"));
        QCOMPARE(qbs::Logger::instance().level(), qbs::LoggerTrace);
    }

    void testInvalidCommandLine()
    {
        qbs::CommandLineOptions options;
        QVERIFY(!options.parseCommandLine(QStringList() << "-x")); // Unknown short option.
        QVERIFY(!options.parseCommandLine(QStringList() << "--xyz")); // Unknown long option.
        QVERIFY(!options.parseCommandLine(QStringList() << "-vjv")); // Invalid position.
        QVERIFY(!options.parseCommandLine(QStringList() << "-j"));  // Missing argument.
        QVERIFY(!options.parseCommandLine(QStringList() << "-j" << "0")); // Wrong argument.
        QVERIFY(!options.parseCommandLine(QStringList() << "--products"));  // Missing argument.
        QVERIFY(!options.parseCommandLine(QStringList() << "--changed-files" << ",")); // Wrong argument.
        QVERIFY(!options.parseCommandLine(QStringList() << "--log-level" << "blubb")); // Wrong argument.
    }

    void testFileInfo()
    {
        QCOMPARE(qbs::FileInfo::fileName("C:/waffl/copter.exe"), QString("copter.exe"));
        QCOMPARE(qbs::FileInfo::baseName("C:/waffl/copter.exe.lib"), QString("copter"));
        QCOMPARE(qbs::FileInfo::completeBaseName("C:/waffl/copter.exe.lib"), QString("copter.exe"));
        QCOMPARE(qbs::FileInfo::path("abc"), QString("."));
        QCOMPARE(qbs::FileInfo::path("/abc/lol"), QString("/abc"));
        QVERIFY(!qbs::FileInfo::isAbsolute("bla/lol"));
        QVERIFY(qbs::FileInfo::isAbsolute("/bla/lol"));
#ifdef Q_OS_WIN
        QVERIFY(qbs::FileInfo::isAbsolute("C:\\bla\\lol"));
#endif
        QCOMPARE(qbs::FileInfo::resolvePath("/abc/lol", "waffl"), QString("/abc/lol/waffl"));
    }

    void testProjectFileLookup()
    {
        const QString srcDir = QLatin1String(SRCDIR);
        const QString noProjectsDir = srcDir + QLatin1String("data/dirwithnoprojects");
        const QString oneProjectDir = srcDir + QLatin1String("data/dirwithoneproject");
        const QString multiProjectsDir = srcDir + QLatin1String("data/dirwithmultipleprojects");
        Q_ASSERT(QDir(noProjectsDir).exists() && QDir(oneProjectDir).exists()
                && QDir(multiProjectsDir).exists());
        qbs::CommandLineOptions options;
        const QStringList args(QLatin1String("-f"));
        QString projectFilePath = multiProjectsDir + QLatin1String("/project.qbp");
        QVERIFY(options.parseCommandLine(args + QStringList(projectFilePath)));
        QCOMPARE(projectFilePath, options.projectFileName());
        projectFilePath = oneProjectDir + QLatin1String("/project.qbp");
        QVERIFY(options.parseCommandLine(args + QStringList(oneProjectDir)));
        QCOMPARE(projectFilePath, options.projectFileName());
        QVERIFY(!options.parseCommandLine(args + QStringList(noProjectsDir)));
        QVERIFY(!options.parseCommandLine(args + QStringList(multiProjectsDir)));
    }
};

QTEST_MAIN(TestTools)

#include "tst_tools.moc"
