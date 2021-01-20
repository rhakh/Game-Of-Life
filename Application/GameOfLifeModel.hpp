#ifndef GAMEOFLIFEMODEL_HPP
#define GAMEOFLIFEMODEL_HPP

#include <QAbstractTableModel>
#include <QtQml/qqml.h>
#include <iostream>
#include <stdint.h>
#include <vector>
#include <memory>
#include <time.h>
#include "AGameOfLife.hpp"

class GameOfLifeModel : public QAbstractTableModel, public AGameOfLife {

    Q_OBJECT
    Q_ENUMS(Roles)


public:
    enum Roles {
        CellRole
    };

    QHash<int, QByteArray> roleNames() const override {
        return {
            { CellRole, "value" }
        };
    }

    GameOfLifeModel(int height = 48, int width = 71, QObject *parent = nullptr);
    GameOfLifeModel(const Map &map, QObject *parent = nullptr);
    ~GameOfLifeModel();

    void liveOneGeneration();
    void liveNGeneration(int num_of_generations);
    Map_ptr liveNGeneration(int argc, char **argv,
                            int num_of_generations,
                            GOF_verbose_lvl verbose) override;

public:
    // QAbstractItemModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    Q_INVOKABLE void nextStep();
    Q_INVOKABLE bool loadFile(const QString &fileName);
    Q_INVOKABLE int  getIteration();

private:
    void setCell(int x, int y) override;
    void clearCell(int x, int y) override;
    void printMap() override;
    void randomFillMap() override;
    int getLiveNeighbours(int x, int y) override;
    bool parseFile(const std::string &fileName);

private:
    Map_ptr mMap;
    Map_ptr mTmp_map;
    int height;
    int width;
    int iteration;
};

#endif // GAMEOFLIFEMODEL_HPP
