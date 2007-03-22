#ifndef SIDUXCC_INFO_H_
#define SIDUXCC_INFO_H_

#include <kcmodule.h>
#include "infodialog.h"
#include "process.h"

class siduxcc_info : public InfoDialog
{
   Q_OBJECT

   public:
      siduxcc_info(QWidget *parent = 0L, const char *name = 0L, const QStringList &foo = QStringList());
      void load();
      void save();
      void defaults();

   private:
      Process* shell;

};

#endif // KNXCC_INFO_H_
