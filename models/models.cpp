#include "models.h"

QVector<int> SqlTm::getCCol() const {
    return cCol;
}
void SqlTm::setCCol(const QVector<int> &value) {
    cCol = value;
}

