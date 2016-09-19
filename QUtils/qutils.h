#ifndef QUTILS_H
#define QUTILS_H

#include <QObject>
#include <QString>
#include <QFileInfo>

namespace QUtils
{
    qint64 fileSize(const QFileInfo &info);
    bool fileIsReadable(const QFileInfo &info);
    QString fileMd5(const QFileInfo &info);

    qint64 dirSize(const QFileInfo &info);
    bool dirIsReadable(const QFileInfo &info);
    bool dirIsEmpty(const QFileInfo &info);

#ifdef Q_OS_MACX
    class QMacUtils : public QObject
    {
        Q_OBJECT

    public:
        /*!
         * \brief constructeur QMacUtils
         *
         * \param parent
         */
        explicit QMacUtils(QObject *parent = 0);

        static int unzip(const QString &zipFilePath, const QString &destinationPath);
    };
#endif

    class QNetworkUtils : public QObject
    {
        Q_OBJECT

    public:
        /*!
         * \brief constructeur QNetworkUtils
         *
         * \param parent
         */
        explicit QNetworkUtils(QObject *parent = 0);

        static QString ipV4();
    };

    class QStringUtils : public QObject
    {
        Q_OBJECT

     public :
        /*!
         * \brief constructeur QDateUtils
         *
         * \param parent
         */
        explicit QStringUtils(QObject *parent = 0);

        static QString removeSlashEnd(QString &str);
        static QString removeSlashFirst(QString &str);
        static QString addSlashEnd(QString &str);
        static QString addSlashFirst(QString &str);
    };

    class QDateUtils : public QObject
    {
        Q_OBJECT

    public:
        /*!
         * \brief constructeur QDateUtils
         *
         * \param parent
         */
        explicit QDateUtils(QObject *parent = 0);

        static const QString DATETIME_CONDENSED;
        static const QString DATETIME_SQLITEFORMAT;
        static const QString DATE_SQLITEFORMAT;
        static const QString TIME_SQLITEFORMAT;
        static const QString DATETIME_FR;
        static const QString DATETIME_FR_FILEFORMT;
    };

    /*!
     * \brief The QByteUtils class
     */
    class QByteUtils : public QObject
    {
        Q_OBJECT

    public:
        /*!
         * \brief constructeur QByteUtils
         *
         * \param parent
         */
        explicit QByteUtils(QObject *parent = 0);

        static const qint8  OCTET       = 8;
        static const qint16 KILOOCTET   = 1024;
        static const qint32 MEGAOCTET   = 1048576;
        static const qint32 GIGAOCTET   = 1073741824;
        static const qint64 TERAOCTET   = 1099511627776;
        static const qint64 PETAOCTET   = 1125899906842624;

        static QString dataMd5(const QByteArray &data);

        static QString octetToString(const qint64 &o);
        static QString octetToString(const qint64 &o, const qint64 &o2);
    };

    /*!
     * \brief The QFileUtils class
     */
    class QFileUtils : public QObject
    {
        Q_OBJECT

    public:

        /*!
         * \brief constructeur QFileUtils
         *
         * \param parent
         */
        explicit QFileUtils(QObject *parent = 0);

        /*!
         * @brief fileSize
         *
         * Appel la méthode size dans un nouveau thread
         *
         * @param info
         * @return qint64
         */
        static qint64 size(const QFileInfo &info);

        /*!
         * @brief fileIsReadable
         *
         * Indique si le fichier peut être lu
         *
         * @param info
         * @return boolean
         */
        static bool isReadable(const QFileInfo &info);

        /*!
         * \brief md5
         *
         * Appel la méthode md5 dans un nouveau thread
         *
         * \param info
         * \return QString
         */
        static QString md5(const QFileInfo &info);

    private:
        /*!
         * \brief privateSize
         *
         * Calcul la taille d'un fichier, ou d'un raccourci sous windows.
         *
         * \return
         */
        static qint64 privateSize(const QFileInfo &info);

        /*!
         * \brief privateMd5
         *
         * Calcul le md5 d'un fichier
         *
         * \return
         */
        static QString privateMd5(const QFileInfo &info);
    };

    /*!
     * \brief The QDirUtils class
     */
    class QDirUtils : public QObject
    {
        Q_OBJECT

    public:

        /*!
         * \brief constructeur QDirUtils
         *
         * \param parent
         */
        explicit QDirUtils(QObject *parent = 0);

        /*!
         * \brief dirIsReadable
         *
         * Indique si le repertoire peut être lu ou non
         *
         * \param info
         * \return boolean
         */
        static bool isReadable(const QFileInfo &info);

        /*!
         * \brief isEmpty
         *
         * Indique si le repertoire est vide, si le repertoire ne peut être lu
         * il sera considéré comme vide.
         *
         * \param info
         * \return boolean
         */
        static bool isEmpty(const QFileInfo &info);

        /*!
         * \brief size
         *
         * Appel la méthode QDirUtils dans un autre thread, avec QtConcurrent::run
         *
         * \param info
         * \return
         */
        static qint64 size(const QFileInfo &info);

        /*!
         * \brief copy
         * Copie le repertoire source vers le repertoire de destination
         * Si le fichier existe il est écrasé
         *
         * \param folderSource
         * \param folderDestination
         */
        static void copy(const QString &folderSource, const QString &folderDestination);

    private:
        /*!
         * \brief privateSize
         *
         * Calcul la taille d'un dossier
         *
         * \param info
         */
        static qint64 privateSize(const QFileInfo &info);
    };
}

#endif // QUTILS_H
