#ifndef SIDUXCC_SERVICES_H_
#define SIDUXCC_SERVICES_H_


#include "displaydialog.h"
#include "process.h"

class siduxcc_services : public DisplayDialog
{
   Q_OBJECT

   public:
      siduxcc_services(QWidget *parent = 0L, const char *name = 0L, const QStringList &foo = QStringList());
      void load();
      void save();
      void defaults();

   private:
      Process* shell;
      QValueList<QString> servicesToEnable;
      QValueList<QString> servicesToDisable;


   public slots:
      virtual void updateSlot();

   protected slots:
      void disableService();
      void enableService();

};

#endif
