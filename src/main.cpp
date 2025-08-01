#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include <QStandardPaths>
#include <QMessageBox>
#include <iostream>
#include "MainWindow.h"
#include "LoginDialog.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application properties
    app.setApplicationName("Trading Desktop App");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Trading Solutions");
    app.setOrganizationDomain("tradingsolutions.com");
    
    // Set application style
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // Set dark theme
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    app.setPalette(darkPalette);
    
    // Create application data directory
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(appDataPath);
    
    try {
        // Show login dialog first
        std::cerr << "Debug: Creating login dialog" << std::endl;
        LoginDialog loginDialog;
        std::cerr << "Debug: Showing login dialog" << std::endl;
        if (loginDialog.exec() != QDialog::Accepted) {
            std::cerr << "Debug: Login dialog cancelled" << std::endl;
            return 0; // User cancelled login
        }
        
        std::cerr << "Debug: Login dialog accepted, getting user" << std::endl;
        // Get authenticated user
        auto user = loginDialog.getAuthenticatedUser();
        if (!user) {
            std::cerr << "Debug: No authenticated user found" << std::endl;
            QMessageBox::critical(nullptr, "Error", "Authentication failed.");
            return 1;
        }
        
        std::cerr << "Debug: User authenticated: " << user->getUsername() << std::endl;
        // Create and show main window
        std::cerr << "Debug: Creating main window" << std::endl;
        MainWindow mainWindow;
        std::cerr << "Debug: Setting user in main window" << std::endl;
        mainWindow.setUser(user);
        std::cerr << "Debug: Showing main window" << std::endl;
        mainWindow.show();
        
        std::cerr << "Debug: Starting event loop" << std::endl;
        return app.exec();
        
    } catch (const std::exception& e) {
        QMessageBox::critical(nullptr, "Fatal Error", 
                            QString("An unexpected error occurred: %1").arg(e.what()));
        return 1;
    }
}




