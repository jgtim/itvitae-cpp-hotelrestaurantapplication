#include "hotel/restaurantwindow/selecttabledialog.h"
#include <QVBoxLayout>
#include <QScrollArea>
#include <QMessageBox>
#include <QPainter>
#include <QMouseEvent>
#include <cmath>

// Inner class - dezelfde als in restaurantwindow.cpp
class SelectableTableCanvas : public QWidget
{
    Q_OBJECT

public:
    explicit SelectableTableCanvas(const QVector<Table>& tables, QWidget *parent = nullptr)
        : QWidget(parent), tables(tables), selectedTable(0)
    {
        updateCanvasSize();
    }

    void updateCanvasSize()
    {
        int maxX = 0;
        int maxY = 0;

        for (const Table& table : tables) {
            if (table.getX() + 50 > maxX) maxX = table.getX() + 50;
            if (table.getY() + 50 > maxY) maxY = table.getY() + 50;
        }
        setMinimumSize(maxX + 0, maxY + 0);
    }

    int getSelectedTable() const {
        return selectedTable;
    }

signals:
    void tableSelected(int tableNumber);

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        painter.setPen(QColor("#1E1E24"));
        painter.setFont(QFont("Arial", 16, QFont::Bold));
        painter.drawText(30, 30, "Inside");

        painter.drawText(30, 245, "Terrace");

        painter.setPen(QPen(QColor("#2D2D35"), 2));
        painter.drawLine(20, 225, width() - 20, 225);

        for (const Table& table : tables) {
            paintTable(table, painter);
        }
    }

    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton) {
            QPoint mousePos = event->pos();

            for (const Table& table : tables) {
                const int radius = 25;
                QPoint center(table.getX() + radius, table.getY() + radius);

                int dx = mousePos.x() - center.x();
                int dy = mousePos.y() - center.y();
                double distance = std::sqrt(dx * dx + dy * dy);

                if (distance <= radius) {
                    selectedTable = table.getNumber();
                    emit tableSelected(selectedTable);
                    update();
                    break;
                }
            }
        }

        QWidget::mousePressEvent(event);
    }

private:
    QVector<Table> tables;
    int selectedTable;

    void paintTable(const Table& table, QPainter& painter)
    {
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
        if (table.getNumber() == selectedTable) {
            painter.setPen(QPen(Qt::black, 3));
        } else {
            painter.setPen(QPen(QColor("#2D2D35"), 2));
        }
        painter.drawEllipse(table.getX(), table.getY(), 50, 50);

        painter.setPen(textColor);
        painter.drawText(table.getX(), table.getY(), 50, 50,
                         Qt::AlignCenter,
                         QString::number(table.getNumber()));
    }
};

SelectTableDialog::SelectTableDialog(const QVector<Table>& tables, QWidget *parent)
    : QDialog(parent), selectedTableNumber(0)
{
    setWindowTitle("Select a Table");
    setMinimumSize(700, 500);

    SelectableTableCanvas *canvas = new SelectableTableCanvas(tables, this);
    tableCanvas = canvas;

    connect(canvas, QOverload<int>::of(&SelectableTableCanvas::tableSelected),
            this, [this]() {
                selectedTableNumber = qobject_cast<SelectableTableCanvas*>(tableCanvas)->getSelectedTable();
                accept();
            });

    setupLayout();
}

SelectTableDialog::~SelectTableDialog()
{
}

void SelectTableDialog::setupLayout()
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidget(tableCanvas);
    scrollArea->setWidgetResizable(true);
    tableCanvas->setStyleSheet("background-color: #F8F9FA; border: 2px solid #2D2D35;");

    layout->addWidget(scrollArea);
    setLayout(layout);
}

int SelectTableDialog::getSelectedTable() const
{
    return selectedTableNumber;
}

#include "selecttabledialog.moc"