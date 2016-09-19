#include "qutils.h"

#include <QFile>
#include <QDebug>
#include <QProcess>
#include <QDirIterator>
#include <QCryptographicHash>
#include <QtConcurrent/QtConcurrentRun>

#include <QHostAddress>
#include <QNetworkInterface>

#ifdef Q_OS_WIN32
    #include "windows.h"
#endif

namespace QUtils
{
    qint64 fileSize(const QFileInfo &info)
    {
        return QFileUtils::size(info);
    }

    bool fileIsReadable(const QFileInfo &info)
    {
        return QFileUtils::isReadable(info);
    }

    QString fileMd5(const QFileInfo &info)
    {
        return QFileUtils::md5(info);
    }

    qint64 dirSize(const QFileInfo &info)
    {
        return QDirUtils::size(info);
    }

    bool dirIsReadable(const QFileInfo &info)
    {
        return QDirUtils::isReadable(info);
    }

    bool dirIsEmpty(const QFileInfo &info)
    {
        return QDirUtils::isEmpty(info);
    }

#ifdef Q_OS_MACX
    int QMacUtils::unzip(const QString &zipFilePath, const QString &destinationPath)
    {
        QString cmd = tr("unzip -q -o \"%1\" -d \"%2\"")
                .arg(zipFilePath).arg(destinationPath);

        return QProcess::execute(cmd);
    }
#endif

    QString QNetworkUtils::ipV4()
    {
        QString ipV4;

        foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {

            if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost)) {
                ipV4 = address.toString();
            }
        }

        return ipV4;
    }

    QString QStringUtils::removeSlashEnd(QString &str)
    {
        if (!str.isEmpty()) {
            int strLength(str.count()-1);
            const QChar lastCharacter(str.at(strLength));

            if (lastCharacter == '/') { str.remove(strLength); }
        }

        return str;
    }

    QString QStringUtils::removeSlashFirst(QString &str)
    {
        if (!str.isEmpty() && str.at(0) == '/') {
            str.remove(0, 1);
        }

        return str;
    }

    QString QStringUtils::addSlashEnd(QString &str)
    {
        // Ajoute un slash à la fin de la chaine
        if (str.isEmpty() || str.at(str.count()-1) != '/') {
            str += "/";
        }

        return str;
    }

    QString QStringUtils::addSlashFirst(QString &str)
    {
        if (str.isEmpty()) {
            str = "/";
        } else if (str.at(0) != '/') {
            str = "/" + str;
        }

        return str;
    }

    const QString QDateUtils::DATETIME_CONDENSED    = "yyyyMMddhhmmss";
    const QString QDateUtils::DATETIME_SQLITEFORMAT = "yyyy-MM-dd hh:mm:ss";
    const QString QDateUtils::DATE_SQLITEFORMAT     = "yyyy-MM-dd";
    const QString QDateUtils::TIME_SQLITEFORMAT     = "hh:mm:ss";
    const QString QDateUtils::DATETIME_FR           = "dd-MM-yyyy hh:mm:ss";
    const QString QDateUtils::DATETIME_FR_FILEFORMT = "dd-MM-yyyy-hh-mm-ss";

    QString QByteUtils::dataMd5(const QByteArray &data)
    {
        QCryptographicHash dataCrypto(QCryptographicHash::Md5);
        dataCrypto.addData(data);

        return dataCrypto.result().toHex();
    }

    QString QByteUtils::octetToString(const qint64 &o)
    {
        QString unit;

        double  size(o);

        if (size < KILOOCTET) {
            unit = "o";
        } else if (size < MEGAOCTET) {
            size /= (double) KILOOCTET;
            unit = "ko";
        } else if (size < GIGAOCTET){
            size /= (double) MEGAOCTET;
            unit = "Mo";
        } else if (size < TERAOCTET){
            size /= (double) GIGAOCTET;
            unit = "Go";
        } else {
            size /= (double) TERAOCTET;
            unit = "To";
        }

        return QString::number(size).setNum(size, 'g', 4)+" "+unit;
    }

    QString QByteUtils::octetToString(const qint64 &o, const qint64 &o2)
    {
        QString unit, v, v2;

        double  size(o), size2(o2);

        if (size < KILOOCTET) {
            unit = "o";
        } else if (size < MEGAOCTET) {
            size /= (double) KILOOCTET;
            size2 /= (double) KILOOCTET;
            unit = "ko";
        } else if (size < GIGAOCTET){
            size /= (double) MEGAOCTET;
            size2 /= (double) MEGAOCTET;
            unit = "Mo";
        } else if (size < TERAOCTET){
            size /= (double) GIGAOCTET;
            size2 /= (double) GIGAOCTET;
            unit = "Go";
        } else {
            size /= (double) TERAOCTET;
            size2 /= (double) TERAOCTET;
            unit = "To";
        }

        v = QString::number(size).setNum(size, 'f', 1);
        v2 = QString::number(size2).setNum(size2, 'f', 1);

        return v+" / "+v2+" "+unit;
    }

    // QFileUtils

    QFileUtils::QFileUtils(QObject *parent) : QObject(parent)
    {

    }

    qint64 QFileUtils::size(const QFileInfo &info)
    {
        QFuture<qint64> future;

        try {

            future = QtConcurrent::run(QFileUtils::privateSize, info);
            future.waitForFinished();

        } catch (std::exception &ex) {
            return 0;
        }

        return future.result();
    }

    bool QFileUtils::isReadable(const QFileInfo &info)
    {
        bool isReadable(false);

        // On ne peut pas ouvrir les liens symboliques
        #if !defined(Q_OS_WIN)
            if (info.isSymLink()) {
                return isReadable;
            }
        #endif

        QFile file(info.filePath());

        if (file.open(QIODevice::ReadOnly)) {
            isReadable = true;
            file.close();
        }

        return isReadable;
    }

    QString QFileUtils::md5(const QFileInfo &info)
    {
        QFuture<QString> future;

        try {

            future = QtConcurrent::run(QFileUtils::privateMd5, info);
            future.waitForFinished();

        } catch (std::exception &ex) {
            return QString();
        }

        return future.result();
    }

    qint64 QFileUtils::privateSize(const QFileInfo &info)
    {
        qint64 size(0);

        if (info.isSymLink()) {

            // On calcule la taille du raccourci
            #if defined(Q_OS_WIN)
                QFile symlink(info.filePath());
                if (symlink.open(QIODevice::ReadOnly)) {

                    size = symlink.readAll().size();
                    symlink.close();

                } else  {
                    qDebug() << QObject::tr("Impossible d'ouvrir le fichier %1 !").arg(info.filePath());
                }
            #endif

        } else {
            size = info.size();
        }

        return size;
    }

    QString QFileUtils::privateMd5(const QFileInfo &info)
    {
        QFile file(info.filePath());
        QCryptographicHash crypto(QCryptographicHash::Md5);

        if (file.open(QIODevice::ReadOnly)) {

            while (not file.atEnd()) {

                QByteArray data(file.read(QByteUtils::MEGAOCTET));

                // Si on a pas réussi a lire le fichier
                if (data.size() == 0) {
                    file.close();
                    throw QObject::tr("Impossible de lire le fichier : %1").arg(info.filePath());
                }

                crypto.addData(data);
            }
            file.close();

        } else {
            throw QObject::tr("Impossible d'ouvrir le fichier : %1").arg(info.filePath());
        }

        return crypto.result().toHex();
    }

    // QDir QDirUtils

    QDirUtils::QDirUtils(QObject *parent) : QObject(parent)
    {

    }

    bool QDirUtils::isReadable(const QFileInfo &info)
    {
        QDir dir(info.filePath());
        QDirIterator it(dir.absolutePath(), QDirIterator::Subdirectories);

        // Si on peut parcourir le repertoire hasNext = true
        return it.hasNext();
    }

    bool QDirUtils::isEmpty(const QFileInfo &info)
    {
        bool isEmpty(true);

        QDir dir(info.filePath());
        QDirIterator it(dir.absolutePath(),
                        QDir::NoDotAndDotDot|QDir::Drives|QDir::System|QDir::Dirs|QDir::Files|QDir::Hidden);

        while (it.hasNext()) {
            // On a trouvé un fichier le repertoire n'est pas vide
            isEmpty = false;
            break;
        }

        return isEmpty;
    }

    qint64 QDirUtils::size(const QFileInfo &info)
    {
        QFuture<qint64> future;

        try {

            future = QtConcurrent::run(QDirUtils::privateSize, info);
            future.waitForFinished();

        } catch (std::exception &ex) {
            Q_UNUSED(ex);
            return 0;
        }

        return future.result();
    }

    void QDirUtils::copy(const QString &folderSource, const QString &folderDestination)
    {
        QDir destinationDir(folderDestination);
        QDir sourceDir(folderSource);

        // Vérifie que les repertoires soient valides
        if (folderSource.isEmpty()) {
            throw QObject::tr("Le chemin du repertoire source ne peut être vide.");
        }

        if (!sourceDir.exists()) {
            throw QObject::tr("Le repertoire source n'existe pas.");
        }

        if (folderDestination.isEmpty()) {
            throw QObject::tr("Le chemin du repertoire de destination ne peut être vide.");
        }

        // Si le repertoire de destination n'existe pas, on le crée
        if (!destinationDir.exists()) {
            if (!destinationDir.mkpath(destinationDir.absolutePath())) {
                throw QObject::tr("Impossible de créer le repertoire de destionnation : %1")
                        .arg(destinationDir.absolutePath());
            }
        }

        // Parcour le dossier source
        QDirIterator it(folderSource, QDir::NoDotAndDotDot|QDir::Drives|QDir::System|QDir::Dirs|QDir::Files|QDir::Hidden,
                        QDirIterator::Subdirectories);

        while (it.hasNext()) {

            it.next();
            QFileInfo infoSource(it.fileInfo());

            if (infoSource.isFile()) {
                QString relativePath(sourceDir.relativeFilePath(infoSource.filePath()));
                QFileInfo destinationInfo(destinationDir.filePath(relativePath));

                QFile fileSource(infoSource.filePath());

                // Création des sous répertoires
                QDir().mkpath(destinationInfo.absolutePath());

                // Si le fichier existe on le supprime
                if (destinationInfo.exists()) {

                    if (!QFile::remove(destinationInfo.filePath())) {

                        throw QObject::tr("Impossible de remplacer le fichier %1 !")
                                .arg(destinationInfo.filePath());
                    }
                }

                if (!fileSource.copy(destinationInfo.filePath())) {

                    throw QObject::tr("La copie du fichier %1 a échoué ! Erreur : %2")
                            .arg(destinationInfo.filePath())
                            .arg(fileSource.errorString());
                }
            }
        }
    }

    qint64 QDirUtils::privateSize(const QFileInfo &info)
    {
        QDir    dir(info.filePath());
        qint64  size(0); // Taille du repertoire en octet

        QDirIterator it(dir.absolutePath(), QDir::NoDotAndDotDot|QDir::Drives|QDir::System|QDir::Dirs|QDir::Files|QDir::Hidden,
                        QDirIterator::Subdirectories);

        while (it.hasNext()) {
            it.next();

            QFileInfo fileInfo(it.fileInfo());
            if (fileInfo.isFile() || fileInfo.isSymLink()) {
                size += fileSize(fileInfo);
            }
        }

        return size;
    }
}
