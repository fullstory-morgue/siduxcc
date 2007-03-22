#ifndef SIDUXCC_NETWORK_H_
#define SIDUXCC_NETWORK_H_

#include "displaydialog.h"
#include "process.h"

class siduxcc_network : public DisplayDialog
{
	Q_OBJECT

	public:
		siduxcc_network(QWidget *parent = 0L, const char *name = 0L, const QStringList &foo = QStringList());
		void load();
		void save();
		void defaults();
	
	private:
		Process* shell;
	
	public slots:
		virtual void nwSlot();
		virtual void nwlSlot();
		virtual void m1Slot();
		virtual void m2Slot();
		virtual void m3Slot();
		virtual void m4Slot();
		virtual void d1Slot();
		virtual void d2Slot();
		virtual void d3Slot();
		virtual void i1Slot();

		virtual void getBrowsers();
		virtual void getNetworkcards();

		virtual void ncDescSlot();
		virtual void ncConfigSlot();
		virtual void ncEnableSlot();
		virtual void ncDisableSlot();
	
};

#endif
