#ifndef MUMBLE_MUMBLE_PTTBUTTONWIDGET_H_
#define MUMBLE_MUMBLE_PTTBUTTONWIDGET_H_

#include "ui_PTTButtonWidget.h"

class PTTButtonWidget : public QWidget, public Ui::qwPTTButtonWidget {
		Q_OBJECT
		Q_DISABLE_COPY(PTTButtonWidget)
	protected:
		void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
	public:
		PTTButtonWidget(QWidget *parent = 0);
	public slots:
		void on_qpbPushToTalk_pressed();
		void on_qpbPushToTalk_released();
	signals:
		void triggered(bool checked, QVariant);
};

#endif  // PTTBUTTONWIDGET_H_
