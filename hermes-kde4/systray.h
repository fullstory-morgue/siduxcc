 #ifndef SYSTRAY_H
 #define SYSTRAY_H

 #include <QSystemTrayIcon>
 #include <QWidget>

 class QAction;
 class QCheckBox;
 class QComboBox;
 class QGroupBox;
 class QLabel;
 class QLineEdit;
 class QMenu;
 class QPushButton;
 class QSpinBox;
 class QTextEdit;

 class SysTray : public QObject
 {
     Q_OBJECT

	public:
		SysTray();
		
	public slots:
		void updateIcon();
	 
	private:
		QSystemTrayIcon *trayIcon;




 };

 #endif 
