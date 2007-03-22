#ifndef SIDUXCC_DISPLAY_H_
#define SIDUXCC_DISPLAY_H_


#include "displaydialog.h"
#include "process.h"

class siduxcc_display : public DisplayDialog
{
	Q_OBJECT

	public:
		siduxcc_display(QWidget *parent = 0L, const char *name = 0L, const QStringList &foo = QStringList());
		void load();
		void save();
		void defaults();
		void backup();

	private:
		Process* shell;

	public slots:
		virtual void fontSlot();
		virtual void mlSlot();
		virtual void restartxSlot();

};

#endif
