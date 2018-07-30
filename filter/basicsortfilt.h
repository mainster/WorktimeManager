#ifndef BASICSORTFILT_H
#define BASICSORTFILT_H


#include <QWidget>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QCheckBox;
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QSortFilterProxyModel;
class QTreeView;
QT_END_NAMESPACE



class BasicSortFilt : public QWidget {

	Q_OBJECT

public:
	BasicSortFilt(QWidget *parent = nullptr);

	void setSourceModel(QAbstractItemModel *model);

	QAbstractItemModel *createMailModel(QObject *parent);
	void addMail(QAbstractItemModel *model, const QString &subject, const QString &sender, const QDateTime &date);
private slots:
	void filterRegExpChanged();
	void filterColumnChanged();
	void sortChanged();

private:
	void createUi(QSortFilterProxyModel *proxyModel);
	void connectUi();

	QSortFilterProxyModel *proxyModel;
	QGroupBox *sourceGroupBox;
	QGroupBox *proxyGroupBox;
	QTreeView *sourceView;
	QTreeView *proxyView;
	QCheckBox *filterCaseSensitivityCheckBox;
	QCheckBox *sortCaseSensitivityCheckBox;
	QLabel *filterPatternLabel;
	QLabel *filterSyntaxLabel;
	QLabel *filterColumnLabel;
	QLineEdit *filterPatternLineEdit;
	QComboBox *filterSyntaxComboBox;
	QComboBox *filterColumnComboBox;
};

#endif // BASICSORTFILT_H
