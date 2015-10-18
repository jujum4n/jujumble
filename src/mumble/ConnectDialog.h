/* Copyright (C) 2005-2011, Thorvald Natvig <thorvald@natvig.com>

   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.
   - Neither the name of the Mumble Developers nor the names of its
     contributors may be used to endorse or promote products derived from this
     software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MUMBLE_MUMBLE_CONNECTDIALOG_H_
#define MUMBLE_MUMBLE_CONNECTDIALOG_H_

#ifndef Q_MOC_RUN
# include <boost/accumulators/accumulators.hpp>
# include <boost/accumulators/statistics/stats.hpp>
#endif

#include <QtCore/QtGlobal>
#include <QtCore/QString>
#include <QtCore/QUrl>
#if QT_VERSION >= 0x050000
# include <QtWidgets/QTreeWidget>
#else
# include <QtGui/QTreeWidget>
#endif

#include <QtNetwork/QHostInfo>

#ifdef USE_BONJOUR
#include <dns_sd.h>
#endif

#include "BonjourRecord.h"
#include "Net.h"
#include "Timer.h"

struct FavoriteServer;
class QUdpSocket;

typedef QPair<QHostAddress, unsigned short> qpAddress;

struct PublicInfo {
	QString qsName;
	QUrl quUrl;
	QString qsIp;
	QString qsCountry;
	QString qsCountryCode;
	QString qsContinentCode;
	unsigned short usPort;
	bool bCA;
};

struct PingStats {
private:
	Q_DISABLE_COPY(PingStats)
protected:
	void init();
public:
	quint32 uiVersion;
	quint32 uiPing;
	quint32 uiPingSort;
	quint32 uiUsers;
	quint32 uiMaxUsers;
	quint32 uiBandwidth;
	quint32 uiSent;
	quint32 uiRecv;

	double dPing;

	typedef boost::accumulators::accumulator_set<double, boost::accumulators::stats<boost::accumulators::tag::count, boost::accumulators::tag::extended_p_square> > asQuantileType;
	asQuantileType *asQuantile;

	void reset();

	PingStats();
	~PingStats();
};

class ServerItem;

class ServerView : public QTreeWidget {
		Q_OBJECT
		Q_DISABLE_COPY(ServerView)
	public:
		ServerItem *siFavorite, *siLAN, *siPublic;
		QMap<QString, QString> qmContinentNames;
		QMap<QString, ServerItem *> qmCountry;
		QMap<QString, ServerItem *> qmContinent;

		ServerView(QWidget *);
		~ServerView() Q_DECL_OVERRIDE;

		void fixupName(ServerItem *si);

		ServerItem *getParent(const QString &continent, const QString &countrycode, const QString &countryname, const QString &usercontinentcode, const QString &usercountrycode);
	protected:
		QMimeData *mimeData(const QList<QTreeWidgetItem *>) const Q_DECL_OVERRIDE;
		QStringList mimeTypes() const Q_DECL_OVERRIDE;
		Qt::DropActions supportedDropActions() const Q_DECL_OVERRIDE;
		bool dropMimeData(QTreeWidgetItem *, int, const QMimeData *, Qt::DropAction) Q_DECL_OVERRIDE;
};

#include "ui_ConnectDialog.h"
#include "ui_ConnectDialogEdit.h"

class ServerItem : public QTreeWidgetItem, public PingStats {
		Q_DISABLE_COPY(ServerItem)
	protected:
		void init();
	public:
		enum ItemType { FavoriteType, LANType, PublicType };

		static QMap<QString, QIcon> qmIcons;

		bool bParent;
		ServerItem *siParent;
		QList<ServerItem *> qlChildren;

		QString qsName;

		QString qsHostname;
		unsigned short usPort;
		bool bCA;

		QString qsUsername;
		QString qsPassword;

		QString qsCountry;
		QString qsCountryCode;
		QString qsContinentCode;

		QString qsUrl;

		QString qsBonjourHost;
		BonjourRecord brRecord;

		QList<QHostAddress> qlAddresses;

		ItemType itType;

		ServerItem(const FavoriteServer &fs);
		ServerItem(const PublicInfo &pi);
		ServerItem(const QString &name, const QString &host, unsigned short port, const QString &uname, const QString &password = QString());
		ServerItem(const BonjourRecord &br);
		ServerItem(const QString &name, ItemType itype, const QString &continent = QString(), const QString &country = QString());
		ServerItem(const ServerItem *si);
		~ServerItem();

		/// Converts given mime data into a ServerItem object
		///
		/// This function checks the clipboard for a valid mumble:// style
		/// URL and converts it into a ServerItem ready to add to the connect
		/// dialog. It also parses .lnk files of InternetShortcut/URL type
		/// to enable those to be dropped onto the clipboard.
		///
		/// @note If needed can query the user for a user name using a modal dialog.
		/// @note If a server item is returned it's the callers reponsibility to delete it.
		///
		/// @param mime Mime data to analyze
		/// @param default_name If true the hostname is set as item name if none is given
		/// @param p Parent widget to use in case the user has to be queried
		/// @return Server item or NULL if mime data invalid.
		///
		static ServerItem *fromMimeData(const QMimeData *mime, bool default_name = true, QWidget *p = NULL);

		void addServerItem(ServerItem *child);

		FavoriteServer toFavoriteServer() const;
		QMimeData *toMimeData() const;
		static QMimeData *toMimeData(const QString &name, const QString &host, unsigned short port, const QString &channel = QString());

		static QIcon loadIcon(const QString &name);

		void setDatas(double ping = 0.0, quint32 users = 0, quint32 maxusers = 0);
		bool operator< (const QTreeWidgetItem &) const Q_DECL_OVERRIDE;

		QVariant data(int column, int role) const Q_DECL_OVERRIDE;

		void hideCheck();
};

class ConnectDialogEdit : public QDialog, protected Ui::ConnectDialogEdit {
	private:
		Q_OBJECT
		Q_DISABLE_COPY(ConnectDialogEdit)
	protected:
		bool bOk;
		bool bCustomLabel;
	public slots:
		void validate();
		void accept();

		void on_qcbShowPassword_toggled(bool);
		void on_qleName_textEdited(const QString&);
		void on_qleServer_textEdited(const QString&);
	public:
		QString qsName, qsHostname, qsUsername, qsPassword;
		unsigned short usPort;
		ConnectDialogEdit(QWidget *parent,
		                  const QString &name = QString(),
		                  const QString &host = QString(),
		                  const QString &user = QString(),
		                  unsigned short port = DEFAULT_MUMBLE_PORT,
		                  const QString &password = QString(),
		                  bool add = false);
};

class ConnectDialog : public QDialog, public Ui::ConnectDialog {
		friend class ServerView;
	private:
		Q_OBJECT
		Q_DISABLE_COPY(ConnectDialog)
	protected:
		static QList<PublicInfo> qlPublicServers;
		static QString qsUserCountry, qsUserCountryCode, qsUserContinentCode;
		static Timer tPublicServers;

		QMenu *qmPopup, *qmFilters;
		QActionGroup *qagFilters;
		QPushButton *qpbEdit;

		bool bPublicInit;
		bool bAutoConnect;

		Timer tPing;
		Timer tCurrent, tHover, tRestart;
		QUdpSocket *qusSocket4;
		QUdpSocket *qusSocket6;
		QTimer *qtPingTick;
		QList<ServerItem *> qlItems;

		ServerItem *siAutoConnect;

		QList<QString> qlDNSLookup;
		QSet<QString> qsDNSActive;
		QHash<QString, QSet<ServerItem *> > qhDNSWait;
		QHash<QString, QList<QHostAddress> > qhDNSCache;

		QHash<qpAddress, quint64> qhPingRand;
		QHash<qpAddress, QSet<ServerItem *> > qhPings;

		QMap<QPair<QString, unsigned short>, unsigned int> qmPingCache;

		bool bIPv4;
		bool bIPv6;
		int iPingIndex;

		bool bLastFound;

		QMap<QString, QIcon> qmIcons;

		void sendPing(const QHostAddress &, unsigned short port);

		void initList();
		void fillList();

		void startDns(ServerItem *);
		void stopDns(ServerItem *);
	public slots:
		void accept();
		void fetched(QByteArray, QUrl, QMap<QString, QString>);

		void udpReply();
		void lookedUp(QHostInfo);
		void timeTick();

		void on_qaFavoriteAdd_triggered();
		void on_qaFavoriteAddNew_triggered();
		void on_qaFavoriteEdit_triggered();
		void on_qaFavoriteRemove_triggered();
		void on_qaFavoriteCopy_triggered();
		void on_qaFavoritePaste_triggered();
		void on_qaUrl_triggered();
		void onFiltersTriggered(QAction *);
		void on_qtwServers_currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *);
		void on_qtwServers_itemDoubleClicked(QTreeWidgetItem *, int);
		void on_qtwServers_customContextMenuRequested(const QPoint &);
		void on_qtwServers_itemExpanded(QTreeWidgetItem *);
		void OnSortChanged(int, Qt::SortOrder);
	public:
		QString qsServer, qsUsername, qsPassword;
		unsigned short usPort;
		ConnectDialog(QWidget *parent, bool autoconnect);
		~ConnectDialog();

#ifdef USE_BONJOUR
	protected:
		QList<BonjourRecord> qlBonjourActive;
	public slots:
		void onUpdateLanList(const QList<BonjourRecord> &);
		void onLanBrowseError(DNSServiceErrorType);

		void onResolved(BonjourRecord, QString, int);
		void onLanResolveError(BonjourRecord, DNSServiceErrorType);
#endif
};

#endif
