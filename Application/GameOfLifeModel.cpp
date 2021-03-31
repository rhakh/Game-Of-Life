/*
#include "gameoflifemodel.h"
#include <QFile>
#include <QTextStream>
#include <QRect>

GameOfLifeModel::GameOfLifeModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    clear();
}

//! [modelsize]
int GameOfLifeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return height;
}

int GameOfLifeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return width;
}
//! [modelsize]

//! [read]
QVariant GameOfLifeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != CellRole)
        return QVariant();

    return QVariant(m_currentState[cellIndex({index.column(), index.row()})]);
}
//! [read]

//! [write]
bool GameOfLifeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != CellRole || data(index, role) == value)
        return false;

    m_currentState[cellIndex({index.column(), index.row()})] = value.toBool();
    emit dataChanged(index, index, {role});

    return true;
}
//! [write]

Qt::ItemFlags GameOfLifeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable;
}

//! [update]
void GameOfLifeModel::nextStep()
{
    StateContainer newValues;

    for (std::size_t i = 0; i < size; ++i) {
        bool currentState = m_currentState[i];

        int cellNeighborsCount = this->cellNeighborsCount(cellCoordinatesFromIndex(static_cast<int>(i)));

        newValues[i] = currentState == true
                ? cellNeighborsCount == 2 || cellNeighborsCount == 3
                : cellNeighborsCount == 3;
    }

    m_currentState = std::move(newValues);

    emit dataChanged(index(0, 0), index(height - 1, width - 1), {CellRole});
}
//! [update]

//! [loader]
bool GameOfLifeModel::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;

    QTextStream in(&file);
    loadPattern(in.readAll());

    return true;
}

void GameOfLifeModel::loadPattern(const QString &plainText)
{
    clear();

    QStringList rows = plainText.split("\n");
    QSize patternSize(0, rows.count());
    for (QString row : rows) {
        if (row.size() > patternSize.width())
            patternSize.setWidth(row.size());
    }

    QPoint patternLocation((width - patternSize.width()) / 2, (height - patternSize.height()) / 2);

    for (int y = 0; y < patternSize.height(); ++y) {
        const QString line = rows[y];

        for (int x = 0; x < line.length(); ++x) {
            QPoint cellPosition(x + patternLocation.x(), y + patternLocation.y());
            m_currentState[cellIndex(cellPosition)] = line[x] == 'O';
        }
    }

    emit dataChanged(index(0, 0), index(height - 1, width - 1), {CellRole});
}
//! [loader]

void GameOfLifeModel::clear()
{
    m_currentState.fill(false);
    emit dataChanged(index(0, 0), index(height - 1, width - 1), {CellRole});
}

int GameOfLifeModel::cellNeighborsCount(const QPoint &cellCoordinates) const
{
    int count = 0;

    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            if (x == 0 && y == 0)
                continue;

            const QPoint neighborPosition { cellCoordinates.x() + x, cellCoordinates.y() + y };
            if (!areCellCoordinatesValid(neighborPosition))
                continue;

            if (m_currentState[cellIndex(neighborPosition)])
                ++count;

            if (count > 3)
                return count;
        }
    }

    return count;
}

bool GameOfLifeModel::areCellCoordinatesValid(const QPoint &coordinates)
{
    return QRect(0, 0, width, height).contains(coordinates);
}

QPoint GameOfLifeModel::cellCoordinatesFromIndex(int cellIndex)
{
    return {cellIndex % width, cellIndex / width};
}

std::size_t GameOfLifeModel::cellIndex(const QPoint &coordinates)
{
    return std::size_t(coordinates.y() * width + coordinates.x());
}
*/


#include "GameOfLifeModel.hpp"
#include <sstream>
#include <thread>
#include <algorithm>
#include <sstream>
#include <QFile>
#include <QUrl>
#include <QDir>
#include <iostream>
#include <fstream>
#include <regex>
#include <boost/lexical_cast.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/phoenix/bind/bind_function.hpp>
#include <boost/config/warning_disable.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/optional/optional_io.hpp>

GameOfLifeModel::GameOfLifeModel(const Map &map, QObject *parent)
    : QAbstractTableModel(parent), height(map.size()), width(map[0].size()), iteration(0)
{
    this->mMap = std::make_unique<Map>(this->height);
    this->mTmp_map = std::make_unique<Map>(this->height);

    for (int i = 0; i < height; i++) {
        (*this->mMap)[i] = std::vector<uint8_t>(width);
        (*this->mTmp_map)[i] = std::vector<uint8_t>(width);
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (map[y][x] == 1)
                setCell(x, y);
        }
    }
}

GameOfLifeModel::GameOfLifeModel(int height, int width, QObject *parent)
    : QAbstractTableModel(parent), height(height), width(width), iteration(0)
{
    this->mMap = std::make_unique<Map>(this->height);
    this->mTmp_map = std::make_unique<Map>(this->height);

    for (int i = 0; i < height; i++) {
        (*this->mMap)[i] = std::vector<uint8_t>(width);
        (*this->mTmp_map)[i] = std::vector<uint8_t>(width);
    }

    randomFillMap();
}

GameOfLifeModel::~GameOfLifeModel() {}

void GameOfLifeModel::printMap() {
    std::stringstream ss;

    ss << "Generation number = " << iteration << std::endl;
    for (int y = 0; y < height; y++) {
        ss << "[" << y << "]\t";
        for (int x = 0; x < width; x++) {
            if (((*mMap)[y][x] & 0x01) == 1)
                ss << "*";
            else
                ss << ".";
        }
        ss << std::endl;
    }

    std::cout << ss.str();
}

void GameOfLifeModel::liveOneGeneration() {
    int live_cells_cnt;

    // safe copy to mTmp_map
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
            (*mTmp_map)[y][x] = (*mMap)[y][x];

    this->iteration++;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            if ((*mTmp_map)[y][x] == 0)
                continue;

            // Get count of alive neightbour cells
            live_cells_cnt = (*mTmp_map)[y][x] >> 1;

            // Is alive ?
            if ((*mTmp_map)[y][x] & 0x01) {
                // Overpopulation or underpopulation - than die
                if (live_cells_cnt != 2 && live_cells_cnt != 3) {
                    clearCell(x, y);
                }
            } else if (live_cells_cnt == 3) {
                // Has 3 neighbours - than revive
                setCell(x, y);
            }
        }
    }
}

void GameOfLifeModel::liveNGeneration(int num_of_generations)
{
    while (num_of_generations > 0) {
        liveOneGeneration();
        num_of_generations--;
    }

//    auto ret = std::make_unique<Map>(this->height);
//    for (int i = 0; i < height; i++)
//        (*ret)[i] = std::vector<uint8_t>(width);

//    for (int y = 0; y < height; y++)
//        for (int x = 0; x < width; x++)
//            (*ret)[y][x] = (*mMap)[y][x];

    return;
}

Map_ptr GameOfLifeModel::liveNGeneration(int argc, char **argv,
                                    int num_of_generations,
                                    GOF_verbose_lvl verbose)
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);
    Q_UNUSED(num_of_generations);
    Q_UNUSED(verbose);

    std::cout << "Unsported. Use liveNGeneration(int) instead." << std::endl;

    return nullptr;
}

int GameOfLifeModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return height;
}

int GameOfLifeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return width;
}

QVariant GameOfLifeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();

    const auto y = static_cast<int>(index.row());
    const auto x = static_cast<int>(index.column());

    // std::cout << "data: x = " << x << ", y = " << y << std::endl;

    return QVariant((*mMap)[y][x]);
}

bool GameOfLifeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    Q_UNUSED(value);
    if (!index.isValid() || role != Qt::DisplayRole)
        return false;

    const auto y = static_cast<int>(index.row());
    const auto x = static_cast<int>(index.column());

    if ((*mMap)[y][x] & 0x01)
        clearCell(x, y);
    else
        setCell(x, y);

    emit dataChanged(index, index, {role});

    return true;
}

void GameOfLifeModel::nextStep() {
    liveOneGeneration();
    emit dataChanged(index(0, 0), index(height - 1, width - 1), {CellRole});
}

bool GameOfLifeModel::parseFile(const std::string &fileName) {
    namespace qi     = boost::spirit::qi;
    namespace phx    = boost::phoenix;

    std::string     line;
    std::ifstream   file;
    std::regex      comment("#.*");
    std::regex      dimensions("height = [0-9]+, width = [0-9]+");
    std::regex      cell("(\\.|\\*)*");
    int             mapHeight, mapWidth;

    file = std::ifstream(fileName);

    if (!file.good()) {
        // throw CLI_InvalidFile();
        std::cout << "return InvalidFile " << std::endl;
        return false;
    }

    // skip empty lines
    while (std::getline(file, line)) {
        if (std::regex_match(line, comment) || line.empty())
            continue;
        else
            break;
    }

    // parse dimesions line
    auto begin = line.begin();
    auto end = line.end();
    bool ok = qi::phrase_parse
                    (
                        begin,
                        end,
                            "height = " >>
                            qi::int_[phx::ref(mapHeight) = qi::_1] >>
                            ", width = " >>
                            qi::int_[phx::ref(mapWidth) = qi::_1],
                        qi::space
                    );
    if (!ok) {
        // throw CLI_InvalidFile();
        std::cout << "return InvalidFile " << std::endl;
        return false;
    }

    // TODO: make it more beautiful
    // resize and clear map
    (*mMap).resize(mapHeight);
    for (auto &line : (*mMap)) {
        line.resize(mapWidth);
        std::fill(line.begin(), line.end(), 0);
    }
    // resize and clear Tmp_map
    (*mTmp_map).resize(mapHeight);
    for (auto &line : (*mTmp_map)) {
        line.resize(mapWidth);
        std::fill(line.begin(), line.end(), 0);
    }
    this->height = mapHeight;
    this->width = mapWidth;
    this->iteration = 0;

    // parse cells
    int y = 0;
    while (std::getline(file, line)) {
        if (std::regex_match(line, cell)) {
            for (unsigned x = 0; x < line.length(); x++) {
                if (line[x] == '*')
                    setCell(x, y);
            }
        } else {
            // throw CLI_InvalidMap();
            std::cout << "return InvalidMap " << std::endl;
            return false;
        }
        y++;
    }

    return true;
}

QString filterFileName(const QString &urlString) {
    const QUrl url(urlString);

    if (url.isLocalFile())
        return QDir::toNativeSeparators(url.toLocalFile());
    else
        return urlString;
}

bool GameOfLifeModel::loadFile(const QString &filePath) {
    std::cout << "enter to loadFile: " << filePath.toStdString() << std::endl;
    auto res = parseFile(filePath.toStdString());

    emit dataChanged(index(0, 0), index(height - 1, width - 1), {CellRole});

    std::cout << "return res = " << res << std::endl;
    return res;
}

std::string GameOfLifeModel::prepareMap() {
    std::stringstream ss;

    ss << "height = " << height << ", width = " << width << std::endl;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++)
            ss << ((*mMap)[y][x] & 0x01 ? "*" : ".");
        ss << std::endl;
    }

    return ss.str();
}

bool GameOfLifeModel::saveToFile(const QString &filePath) {
    std::string mapStr = prepareMap();
    bool res = false;

    std::cout << "enter to saveToFile: " << filePath.toStdString() << std::endl;

    try {
        std::ofstream file(filePath.toStdString(), std::ofstream::out);
        file << mapStr;
        file.close();
        res = true;
    } catch (const char* msg) {
        std::cerr << "Error: " << msg << std::endl;
    }

    std::cout << "return res = " << res << std::endl;
    return res;
}

int GameOfLifeModel::getIteration()
{
    return iteration;
}

void GameOfLifeModel::_randomFillMap() {
    unsigned seed;
    int x, y;

    // Get seed; random if 0
    seed = (unsigned)time(NULL);
    srand(seed);

    // Randomly initialise cell map with ~50% on pixels
    for (int half_len = (height * width) / 2; half_len > 0; half_len--) {
        x = rand() % (width - 1);
        y = rand() % (height - 1);

        if (((*mMap)[y][x] & 0x01) == 0) {
            setCell(x, y);
        }
    }
}

void GameOfLifeModel::randomFillMap() {
    for (int i = 0; i < this->height; i++)
        std::fill((*mMap)[i].begin(), (*mMap)[i].end(), 0);
    _randomFillMap();
    iteration = 0;
    emit dataChanged(index(0, 0), index(height - 1, width - 1), {CellRole});
}

void GameOfLifeModel::setCell(int x, int y) {
    int x_left, x_right, y_up, y_down;

    x_left = (x == 0) ? width - 1 : x - 1;
    x_right = (x == width - 1) ? 0 : x + 1;
    y_up = (y == 0) ? height - 1 : y - 1;
    y_down = (y == height - 1) ? 0 : y + 1;

    (*mMap)[y][x] |= 0x01;

    (*mMap)[y_up  ][x_left ] += 0x02;
    (*mMap)[y_up  ][x      ] += 0x02;
    (*mMap)[y_up  ][x_right] += 0x02;
    (*mMap)[y     ][x_left ] += 0x02;
    (*mMap)[y     ][x_right] += 0x02;
    (*mMap)[y_down][x_left ] += 0x02;
    (*mMap)[y_down][x      ] += 0x02;
    (*mMap)[y_down][x_right] += 0x02;
}

void GameOfLifeModel::clearCell(int x, int y) {
    int x_left, x_right, y_up, y_down;

    x_left = (x == 0) ? width - 1 : x - 1;
    x_right = (x == width - 1) ? 0 : x + 1;
    y_up = (y == 0) ? height - 1 : y - 1;
    y_down = (y == height - 1) ? 0 : y + 1;

    (*mMap)[y][x] &= ~0x01;

    (*mMap)[y_up  ][x_left ] -= 0x02;
    (*mMap)[y_up  ][x      ] -= 0x02;
    (*mMap)[y_up  ][x_right] -= 0x02;
    (*mMap)[y     ][x_left ] -= 0x02;
    (*mMap)[y     ][x_right] -= 0x02;
    (*mMap)[y_down][x_left ] -= 0x02;
    (*mMap)[y_down][x      ] -= 0x02;
    (*mMap)[y_down][x_right] -= 0x02;
}

int GameOfLifeModel::getLiveNeighbours(int x, int y) {
    return ((*mMap)[y][x] >> 1);
}
