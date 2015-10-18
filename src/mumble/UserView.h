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

#ifndef MUMBLE_MUMBLE_USERVIEW_H_
#define MUMBLE_MUMBLE_USERVIEW_H_

#include <QtCore/QtGlobal>
#if QT_VERSION >= 0x050000
# include <QtWidgets/QStyledItemDelegate>
# include <QtWidgets/QTreeView>
#else 
# include <QtGui/QStyledItemDelegate>
# include <QtGui/QTreeView>
#endif

#include "Timer.h"

class UserDelegate : public QStyledItemDelegate {
	private:
		Q_OBJECT
		Q_DISABLE_COPY(UserDelegate)
	public:
		UserDelegate(QObject *parent = NULL);
		void paint(QPainter * painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

		//! Width/height in px of user/channel flag icons
		const static int FLAG_ICON_DIMENSION;
		//! Padding in px around user/channel flag icons
		const static int FLAG_ICON_PADDING;
		//! Width/height in px of user/channel flags including padding
		const static int FLAG_DIMENSION;

	public slots:
		bool helpEvent(QHelpEvent *event, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &index) Q_DECL_OVERRIDE;
};

class UserView : public QTreeView {
	private:
		Q_OBJECT
		Q_DISABLE_COPY(UserView)
	protected:
		void mouseReleaseEvent(QMouseEvent *) Q_DECL_OVERRIDE;
		void keyPressEvent(QKeyEvent *) Q_DECL_OVERRIDE;
		bool event(QEvent *) Q_DECL_OVERRIDE;

		QTimer *qtSearch;
		QPersistentModelIndex qpmiSearch;
		Timer tSearch;
		QString qsSearch;
	public:
		UserView(QWidget *);
		void keyboardSearch(const QString &search) Q_DECL_OVERRIDE;
#if QT_VERSION >= 0x050000
		void dataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight, const QVector<int> & roles = QVector<int> ()) Q_DECL_OVERRIDE;
#else
		void dataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight) Q_DECL_OVERRIDE;
#endif
		
	public slots:
		void nodeActivated(const QModelIndex &idx);
		void selectSearchResult();
		void updateChannel(const QModelIndex &index);
};

#endif
