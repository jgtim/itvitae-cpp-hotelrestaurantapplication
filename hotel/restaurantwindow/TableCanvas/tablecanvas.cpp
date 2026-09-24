#include "hotel/restaurantwindow/TableCanvas/tablecanvas.h"

#include <QPainter>
#include <QMouseEvent>

#include <QFile>
#include <QDataStream>
#include <QVector>

void TableCanvas::updateCanvasSize() {
        int maxX = 0;
        int maxY = 0;

        for (const Table& table : tables) {
            // Each table ellipse is 50x50 at (x, y)
            if (table.getX() + 50 > maxX) maxX = table.getX() + 50;
            if (table.getY() + 50 > maxY) maxY = table.getY() + 50;
        }
        // Add padding around the canvas edge
        setMinimumSize(maxX + 0, maxY + 0);
}

void TableCanvas::updateTables(QVector<Table> newTables) {
    tables = newTables;
}

int TableCanvas::getSelectedTable() {
    return selectedTable;
}

QVector<Table>* TableCanvas::getTables() {
    return &tables;
}

void TableCanvas::paintEvent(QPaintEvent *event) {
        Q_UNUSED(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);


        painter.setPen(QColor("#1E1E24"));
        painter.setFont(QFont("Arial", 16, QFont::Bold));
        painter.drawText(30, 30, "Inside");


        // =========================
        // Titel TERRAS
        // =========================
        painter.drawText(30, 245, "Terrace");


        // =========================
        // Scheidingslijn
        // =========================
        painter.setPen(QPen(QColor("#2D2D35"), 2));
        painter.drawLine(20, 225, width() - 20, 225);


        // =========================
        // Tafels tekenen
        // =========================
        for (const Table& table : tables) {
            paintTable(table, painter);
        }
}

void TableCanvas::mousePressEvent(QMouseEvent *event) {
        if (event->button() == Qt::LeftButton) {
            QPoint mousePos = event->pos();

            for (Table& table : tables) {
                // Determine circle center and radius based on your drawEllipse dimensions
                const int radius = 25;
                QPoint center(table.getX() + radius, table.getY() + radius);

                // Calculate distance from click point to circle center using Pythagorean theorem
                int dx = mousePos.x() - center.x();
                int dy = mousePos.y() - center.y();
                double distance = std::sqrt(dx * dx + dy * dy);

                // If click is within circle radius, trigger action
                if (distance <= radius) {
                    onTableClicked(table);
                    break; // Stop checking further tables once hit is found
                }
            }
        }
        QWidget::mousePressEvent(event);
}

void TableCanvas::onTableClicked(const Table& table) {
        selectedTable = table.getNumber();

        update();
}

void TableCanvas::paintTable(const Table& table, QPainter& painter) {
        QColor fillColor;
        QColor textColor = QColor("#F5F5F5");

        if (table.getStatus() == TableStatus::FREE) {
            fillColor = QColor("#2ECC71");
            textColor = QColor("#1E1E24");
        }
        else if (table.getStatus() == TableStatus::OCCUPIED) {
            fillColor = QColor("#D9534F");
        }
        else if (table.getStatus() == TableStatus::RESERVED) {
            fillColor = QColor("#E69F00");
        }
        else if (table.getStatus() == TableStatus::OUT_OF_SERVICE) {
            fillColor = QColor("#808080");
        }

        painter.setBrush(fillColor);
        if(table.getNumber() == selectedTable){
            painter.setPen(QPen(Qt::black, 3));
        }else{
            painter.setPen(QPen(QColor("#2D2D35"), 2));
        }
        painter.drawEllipse(table.getX(), table.getY(), 50, 50);

        painter.setPen(textColor);
        painter.drawText(table.getX(), table.getY(), 50, 50,
                         Qt::AlignCenter,
                         QString::number(table.getNumber()));
}

void saveTables(const QVector<Table> &vector, const QString &filePath) {
    QFile file(filePath);

    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);

        // Pass a version to guarantee cross-platform compatibility
        out.setVersion(QDataStream::Qt_6_0);

        // Write the entire vector at once
        out << vector;

        file.close();
    }
}