#define STB_IMAGE_WRITE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS
#include "stb_image_write.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include "lodepng.h"
#include <fstream>
#include <random>
#include <cstdint>
#include <thread>

const std::string VERSION_OF_PROGRAMM = "4.0";

std::random_device rd;

char empty;

float const PI = 3.1410f;

int const STEP_SIZE = 5;
int const CHROMOSOME_SIZE = 31;

int const Y = 4096;
int const X = 4096;

int const LENGHT = 0;
int const LENGHT_DEVIATION = 1;

int const SIZE = 2;
int const SIZE_CHENGES = 3;
int const SIZE_FROM_ANCESTOR = 4;

int const RED = 5;
int const GREEN = 6;
int const BLUE = 7;
int const RED_CHENGES = 8;
int const GREEN_CHENGES = 9;
int const BLUE_CHENGES = 10;
int const COLOR_FROM_ANCESTOR = 11;
int const COLOR_DEVIATION = 12;

int const NUMBER_OF_BRANCHES = 13;
int const ANGLES_OF_BRANCHES1 = 14;
int const ANGLES_OF_BRANCHES2 = 15;
int const ANGLES_OF_BRANCHES3 = 16;
int const ANGLE_DEVIATION = 17;

int const TURN = 18;
int const RANDOM_TURN = 19;
int const DOWN_UP = 20;

int const BRANCHES1 = 21;
int const BRANCHES2 = 22;
int const BRANCHES3 = 23;

int const CHROMOSOME_BRANCHE1 = 24;
int const CHROMOSOME_BRANCHE2 = 25;
int const CHROMOSOME_BRANCHE3 = 26;

int const TYPE_BRANCHE1 = 27;
int const TYPE_BRANCHE2 = 28;
int const TYPE_BRANCHE3 = 29;

int const PLANT_SIZE = 30;

float const SIZE_STEP = 1.0f;
float const KOEF_LEGHT = 1.5f;

class color {
public:
    unsigned char red;
    unsigned char green;
    unsigned char blue;

    color() {
        red = 0;
        green = 0;
        blue = 0;
    }
    color(unsigned char r, unsigned char g, unsigned char b) {
        red = r;
        green = g;
        blue = b;
    }
    void addColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
        red = ((int)(red) * (0xFF - a) + (int)(r)*a) / 0xFF;
        green = ((int)(green) * (0xFF - a) + (int)(g)*a) / 0xFF;
        blue = ((int)(blue) * (0xFF - a) + (int)(b)*a) / 0xFF;
    }
    void addColor(color colory, unsigned char a) {
        red = ((int)(red) * (0xFF - a) + (int)(colory.red) * a) / 0xFF;
        green = ((int)(green) * (0xFF - a) + (int)(colory.green) * a) / 0xFF;
        blue = ((int)(blue) * (0xFF - a) + (int)(colory.blue) * a) / 0xFF;
    }
    void addReduce(char r, char g, char b, unsigned a) {
        red = ((int)(red) * (0xFF - a) + (int)(r)*a) / 0xFF;
        green = ((int)(green) * (0xFF - a) + (int)(g)*a) / 0xFF;
        blue = ((int)(blue) * (0xFF - a) + (int)(b)*a) / 0xFF;
    }
};

class agent {
public:
    color colory; // текущий цвет
    float y; // координата y
    float x; // координата x
    float direction; // текущие направление 
    float radius; // текущий радиус
    int stepNum; // количество шагов
    short index; // номер при рождении
    unsigned char chromosome;
    agent() {
        colory = color();
        y = 0;
        x = 0;
        direction = 0;
        radius = 0;
        stepNum = 0;
        index = 0;
        chromosome = 0;
    }
    agent(float x, float y, color colory, float direction, float radius, float stepNum, short index, unsigned char chromosome) {
        this->colory = colory;
        this->x = x;
        this->y = y;
        this->direction = direction;
        this->radius = radius;
        this->stepNum = stepNum;
        this->index = index;
        this->chromosome = chromosome;
    }
    void operator=(agent age) {
        colory = age.colory;
        x = age.x;
        y = age.y;
        direction = age.direction;
        radius = age.radius;
        stepNum = age.stepNum;
        index = age.index;
        chromosome = age.chromosome;
    }
    void addAngle(float angel) {
        direction += angel;
        if (direction > 1) {
            direction = -2.0f + direction;
        }
        else if (direction < -1) {
            direction = 2.0f + direction;
        }
    }
};

class foreCoor {
public:
    int xStart;
    int yStart;
    int xEnd;
    int yEnd;
    foreCoor() {
        xStart = 0;
        yStart = 0;
        xEnd = 0;
        yEnd = 0;
    }
    foreCoor(int xs, int ys, int xe, int ye) {
        xStart = xs;
        yStart = ys;
        xEnd = xe;
        yEnd = ye;
    }
    void set(int a, int b, int c, int d) {
        xStart = a;
        yStart = b;
        xEnd = c;
        yEnd = d;
    }
};

void genomRand(std::vector<std::vector<unsigned char>>& genom) { // случайный геном
    for (int iy = 0; iy < genom.size(); iy++) {
        for (int ix = 0; ix < genom[iy].size(); ix++) {
            genom[iy][ix] = rand() % 256;
        }
    }
}

void startScreen(std::vector<std::vector<color>>& screen) { // создание чистого экрана
    for (int iy = 0; iy < Y; iy++) {
        for (int ix = 0; ix < X; ix++) {
            screen[iy][ix] = color(0xff, 0xff, 0xff);
        }
    }
}

void drawСircle(color colory, unsigned char alpha, int x, int y, float r, std::vector<std::vector<color>>& screen) { // нарисовать круг на экране
    r = abs(r);
    foreCoor area(x - r - 1, y - r - 1, x + r + 1, y + r + 1); // определение границ круга на экране

    // выписывание круга в границы экрана
    if (area.xStart < 0) area.xStart = 0;
    if (area.yStart < 0) area.yStart = 0;
    if (area.xEnd > X - 1) area.xEnd = X - 1;
    if (area.yEnd > Y - 1) area.yEnd = Y - 1;

    // рисование круга
    for (int iy = area.yStart; iy < area.yEnd; iy++) {
        for (int ix = area.xStart; ix < area.xEnd; ix++) {
            if (sqrt(pow(iy - y, 2) + pow(ix - x, 2)) <= r) {
                screen[iy][ix].addColor(colory, alpha);
            }
        }
    }
}

void creatPNG(std::string currTime, std::vector<std::vector<color>>& screen) {
    std::vector<unsigned char> image(X * Y * 4);
    unsigned int index = 0;
    int r = 0;
    int g = 0;
    int b = 0;
    for (int iy = 0; iy < Y; iy++) {
        for (int ix = 0; ix < X; ix++) {
            index = (iy * X + ix) * 4;
            image[index + 3] = 255;
            r = screen[iy][ix].red;
            g = screen[iy][ix].green;
            b = screen[iy][ix].blue;
            image[index + 0] = r;
            image[index + 1] = g;
            image[index + 2] = b;
        }
    }
    std::string fileName = currTime + ".png";
    unsigned int error = lodepng::encode(fileName.c_str(), image, X, Y);
    if (error) {
        std::cout << "\nerror creating impage/ошибка при создании изображения\n";
    }
    else {
        std::cout << "tree is saved in " + fileName + "/ дерево сохранено в " + fileName + '\n';
    }
}


bool save_as_jpeg(
    const std::vector<std::vector<color>>& image,
    std::string currTime,
    int quality
) {
    if (image.empty() || image[0].empty()) {
        return false;
    }

    // Определяем размеры изображения
    size_t width = image[0].size();
    size_t height = image.size();

    // Создаём буфер для пикселей (RGB)
    std::vector<unsigned char> pixels(width * height * 3);

    // Заполняем буфер данными из вектора
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            size_t idx = (y * width + x) * 3;
            pixels[idx + 0] = image[y][x].red;
            pixels[idx + 1] = image[y][x].green;
            pixels[idx + 2] = image[y][x].blue;
        }
    }

    // Сохраняем с помощью stb_image_write
    // Примечание: stb_image_write.h нужно подключить и скомпилировать вместе с кодом
    int result = stbi_write_jpg((currTime + ".jpg").c_str(), width, height, 3, pixels.data(), quality);

    return result != 0;
}

color colorСhange(color colory, char change) { // изменение яркости
    color out;
    int red;
    int green;
    int blue;
    red = (int)(colory.red) + change;
    green = (int)(colory.green) + change;
    blue = (int)(colory.blue) + change;

    if (red > 0xff) red = 0xff;
    if (red < 0) red = 0;

    if (green > 0xff) green = 0xff;
    if (green < 0) green = 0;

    if (blue > 0xff) blue = 0xff;
    if (blue < 0) blue = 0;

    out = color(red, green, blue);
    return out;
}

void volumetricCircle(color colory, int x, int y, float r, std::vector<std::vector<color>>& screen) { // рисование объёмного круга
    drawСircle(colorСhange(colory, -20), 150, x + 1, y + 1, r, screen); // тёмный круг
    drawСircle(colorСhange(colory, 20), 127, x - 1, y - 1, r, screen); // светлый круг
    drawСircle(colory, 0xFF, x, y, r, screen); // изначальный круг
}

short numOfBrea(unsigned char gen) { // количество потомков агента исходя из гена
    if (gen < 127) {
        return 1;
    }
    else if (gen < 223) {
        return 2;
    }
    else{
        return 3;
    }
}

void loadGenomFromFile(std::vector<std::vector<unsigned char>>& genom, std::string filename) {
    genom.clear();
    std::ifstream file(filename);
    std::string strFromFile;
    std::string temp;
    while (std::getline(file, strFromFile)) {
        if (strFromFile.substr(0, 4) != "seed") {
            genom.push_back({});
            for (int i = 0; i < strFromFile.size(); i++) {
                if (strFromFile[i] != ' ') {
                    temp += strFromFile[i];
                }
                else {
                    genom[genom.size() - 1].push_back(std::stoi(temp));
                    temp.clear();
                }
            }
            if (temp != "") {
                genom[genom.size() - 1].push_back(std::stoi(temp));
                temp.clear();
            }
        }
    }
}

void crossingPlants(std::vector<std::vector<unsigned char>>& finalGenom, std::vector<std::vector<unsigned char>>& genom1, std::vector<std::vector<unsigned char>>& genom2) {
    finalGenom.clear();
    int numChromosomes;
    bool biggestGenom;
    int smallestSize;
    if (genom1.size() > genom2.size()) {
        numChromosomes = genom1.size();
        biggestGenom = false;
        smallestSize = genom2.size();
    }
    else {
        numChromosomes = genom2.size();
        biggestGenom = true;
        smallestSize = genom1.size();
    }
    for (int iy = 0; iy < numChromosomes; iy++) {
        if (iy < smallestSize) {
            finalGenom.push_back({});
            for (int ix = 0; ix < CHROMOSOME_SIZE; ix++) {
                finalGenom[finalGenom.size() - 1].push_back({});
                if (rand() % 2 == 0) {
                    if (genom1.size() - 1 >= ix) {
                        finalGenom[iy][ix] = genom1[iy][ix];
                    }
                    else {
                        finalGenom[iy][ix] = genom2[iy][ix];
                    }
                }
                else {
                    if (genom2.size() - 1 >= ix) {
                        finalGenom[iy][ix] = genom2[iy][ix];
                    }
                    else {
                        finalGenom[iy][ix] = genom1[iy][ix];
                    }
                }
            }
        }
        else {
            if (!biggestGenom) {
                finalGenom.push_back(genom1[iy]);
            }
            else {
                finalGenom.push_back(genom2[iy]);
            }
        }
    }
}

agent loadChromosome(std::vector<std::vector<unsigned char>>& genom, unsigned char numChromosome, int x, int y) {
    agent out = agent(x, y, color(genom[numChromosome][RED], genom[numChromosome][GREEN], genom[numChromosome][BLUE]), 0, (float)(genom[numChromosome][SIZE]) / 30 + 1, (float)(genom[numChromosome][LENGHT]), 0, numChromosome);
    return out;

}

int seedToRandTurn;

bool useRandTurn;

float lastRandTurn; // последнее направление, созданное случайно
unsigned char countStepsAfterCalRandTurn = 0;

void saveGenom(std::vector<std::vector<unsigned char>>& genom, std::string timeStart) {
    std::ofstream gen((timeStart + ".txt").c_str());

    for (int iy = 0; iy < genom.size(); iy++) {
        for (int ix = 0; ix < genom[iy].size(); ix++) {
            if (genom[iy][ix] == 0) {
                genom[iy][ix] = 1;
            }
            if (genom[iy][ix] < 100) {
                gen << 0;
                if (genom[iy][ix] < 10) {
                    gen << 0;
                }
            }
            gen << std::to_string((int)(genom[iy][ix]));
            gen << " ";
        }
        if (iy < genom.size() - 1) {
            gen << "\n";
        }
    }
    gen << "\nseed" << seedToRandTurn;
}

int randToRandTurn() {
    int a = 48271;
    int lastSeedToRandTurn = seedToRandTurn;
    seedToRandTurn = (a * lastSeedToRandTurn) % UINT16_MAX;
    return seedToRandTurn;
}

void loadSeedToRandTurn(std::string filename) {
    std::string lastLine;
    std::ifstream file(filename);
    while (std::getline(file, lastLine));
    if (lastLine.substr(0, 4) == "seed") {
        seedToRandTurn = std::stoi(lastLine.substr(5, lastLine.size() - 5));
    }
    else {
        seedToRandTurn = rd();
    }
}

void addRandTurn(std::vector<std::vector<agent>>& agents, std::vector<std::vector<unsigned char>>& genom, unsigned int il, unsigned int ip) {
    if (countStepsAfterCalRandTurn == 0 or countStepsAfterCalRandTurn >= 50) {
        lastRandTurn = randToRandTurn() % genom[agents[il][ip].chromosome][RANDOM_TURN] - genom[agents[il][ip].chromosome][RANDOM_TURN] / 2;
        lastRandTurn /= 256;
        countStepsAfterCalRandTurn++;
    }
    agents[il][ip].addAngle(lastRandTurn / 30);
}

void createBrea(std::vector<std::vector<agent>>& agents, std::vector<std::vector<unsigned char>>& genom, unsigned int il, unsigned int ip, int breaIndex) {
    float radius;
    color newColor;
    int chromo;
    int index;

    if (breaIndex == 1) {
        chromo = genom[agents[il][ip].chromosome][CHROMOSOME_BRANCHE1];
        index = -1;
    }
    else if (breaIndex == 2) {
        chromo = genom[agents[il][ip].chromosome][CHROMOSOME_BRANCHE2];
        index = 0;
    }
    else {
        chromo = genom[agents[il][ip].chromosome][CHROMOSOME_BRANCHE3];
        index = 1;
    }

    if (chromo != genom.size()) {
        radius = (float)(genom[chromo][SIZE]) / 30
            * (255 - genom[chromo][SIZE_FROM_ANCESTOR]);
        radius += agents[il][ip].radius
            * genom[chromo][SIZE_FROM_ANCESTOR];
        radius /= 255;

        newColor = color(genom[chromo][RED], genom[chromo][GREEN], genom[chromo][BLUE]);
        newColor.addColor(color(agents[il][ip].colory.red, agents[il][ip].colory.green, agents[il][ip].colory.blue), genom[chromo][COLOR_FROM_ANCESTOR]);
        agents[il + 1].push_back(agent(
            agents[il][ip].x,
            agents[il][ip].y,
            newColor,
            agents[il][ip].direction + (float)(genom[agents[il][ip].chromosome][ANGLES_OF_BRANCHES1]) / 256 - 0.5,
            radius,
            genom[chromo][LENGHT],
            index,
            chromo
        )
        );
    }
}

void createBreas(std::vector<std::vector<agent>>& agents, std::vector<std::vector<unsigned char>>& genom, unsigned int il, unsigned int ip) {
    int chromo;
    color newColor;
    float radius = 0;
    int index;

    if (numOfBrea(genom[agents[il][ip].chromosome][NUMBER_OF_BRANCHES]) == 1) {
        createBrea(agents, genom, il, ip, 2);
    }
    else if (numOfBrea(genom[agents[il][ip].chromosome][NUMBER_OF_BRANCHES]) == 2) {
        createBrea(agents, genom, il, ip, 1);
        createBrea(agents, genom, il, ip, 3);

    }
    else if (numOfBrea(genom[agents[il][ip].chromosome][NUMBER_OF_BRANCHES]) == 3) {
        createBrea(agents, genom, il, ip, 1);
        createBrea(agents, genom, il, ip, 2);
        createBrea(agents, genom, il, ip, 3);
    }
}

void changeColor(std::vector<std::vector<agent>>& agents, std::vector<std::vector<unsigned char>>& genom, unsigned int il, unsigned int ip) {
    unsigned char r = genom[agents[il][ip].chromosome][RED_CHENGES];
    unsigned char g = genom[agents[il][ip].chromosome][GREEN_CHENGES];
    unsigned char b = genom[agents[il][ip].chromosome][BLUE_CHENGES];
    agents[il][ip].colory.addColor(color(r, g, b), 255 / agents[il][ip].stepNum - (255 / agents[il][ip].stepNum / 2));

}

void drawPlant(std::vector<std::vector<unsigned char>>& genom, std::vector<std::vector<agent>>& agents, std::vector<std::vector<color>>& screen, int currPlantIndex, int plantCount) {
    time_t timeAfterLastShowPercents = 0;
    for (int il = 0; il < genom[0][PLANT_SIZE] / 15 + 1; il++) {
        if (il > 0) {
            agents[il - 1].clear();
        }
        if (agents[il].size() > 3500) {
            break;
        }
        if (agents[il].size() > 0) {
            for (int ip = 0; ip < agents[il].size(); ip++) {
                if (time(0) - timeAfterLastShowPercents >= 1) {
                    std::cout << "\r" << ip * 100 / agents[il].size() << "% | " << il * 100 / (genom[0][PLANT_SIZE] / 15 + 1) << "% | " << currPlantIndex * 100 / plantCount << "%";
                }
                for (int i = 0; i < agents[il][ip].stepNum; i++) {
                    volumetricCircle(agents[il][ip].colory, agents[il][ip].x, agents[il][ip].y, agents[il][ip].radius, screen);
                    // we determine the coordinates after the agent's step / определяем координаты после шага агента
                    agents[il][ip].x += cosf(agents[il][ip].direction * -PI + PI / 2) * SIZE_STEP;
                    agents[il][ip].y -= sinf(agents[il][ip].direction * -PI + PI / 2) * SIZE_STEP;

                    if (useRandTurn) {
                        addRandTurn(agents, genom, il, ip);
                    }

                    agents[il][ip].addAngle((0.0f - agents[il][ip].direction) * ((float)(char)(genom[agents[il][ip].chromosome][DOWN_UP]) / 127) / 70);

                    agents[il][ip].radius += (float)((char)(genom[agents[il][ip].chromosome][SIZE_CHENGES])) / agents[il][ip].stepNum / 10;
                }

                if (il < genom[0][PLANT_SIZE] / 15) {
                    agents.push_back({});
                    createBreas(agents, genom, il, ip);
                }
            }
        }
        else {
        }
    }

    agents.clear();
}

void start(bool& chromosomeEdit, int& mode, std::string& genName1, std::string& genName2, int& randy, bool& quality, int& fluctuation) {
    std::cout << "1 - создание растений с случайным геномом\n";
    std::cout << "2 - скрещевание двух растений\n";
    std::cout << "3 - создать растение по существующему геному\n";
    std::cout << "4 - сравнение геномов двух растений\n";
    std::string support;
    std::cin >> mode;

    if (mode == 2) {
        std::cout << "Введите названия файла с геномом первого растений\n";
        std::cin >> genName1;
        std::cout << "Введите названия файла с геномом второго растений\n";
        std::cin >> genName2;
    }
    else if (mode == 3) {
        std::cout << "Введите названия файла с геномом растений\n";
        std::cin >> genName1;
    }

    if (mode == 2 or mode == 3) {
        std::cout << "Сколько байт поменять на случайные?\n";
        std::cin >> randy;
        std::cout << "Сколько байт немного изменить?\n";
        std::cin >> fluctuation;
        std::cout << "Включить функцию добавление и убирания случайных хромосом? y/n\n";
        std::cin >> support;
        chromosomeEdit = support[0] == 'y' or support[0] == 'Y';
    }

    if (mode != 4) {
        std::cout << "Использовать ли jpg файлы? Это незначительно ухудшит качество, но значительно уменьшает размер файла y/n\n";
        std::cin >> support;
        quality = !(support[0] == 'y' or support[0] == 'Y');
        std::cout << "Включить ли случайные искрывления веток? y/n\n";
        std::cin >> support;
        useRandTurn = support[0] == 'y' or support[0] == 'Y';
    }
}

int main()
{
    setlocale(LC_ALL, "russian");
    std::cout << "версия: " << VERSION_OF_PROGRAMM << '\n';
    std::random_device rd;
    srand(rd());
    std::vector<std::vector<unsigned char>> genom;
    std::vector<std::vector<agent>> agents;

    std::vector<std::vector<color>> screen = std::vector<std::vector<color>>(Y, std::vector<color>(X));

    std::string genName1;
    std::string genName2;

    std::vector<std::vector<unsigned char>> genom1;
    std::vector<std::vector<unsigned char>> genom2;

    int mode;

    int randy;
    int timeStart = time(0);
    std::string id;

    short index;
    float diretionForBranche = 0.0f;
    int iter;

    int biggestSizeGenom;
    long numOfgens = 0;
    long identicalGens = 0;
    bool chromosomeEdit;
    bool quality;
    int fluctuation;

    start(chromosomeEdit, mode, genName1, genName2, randy, quality, fluctuation);

    if (mode == 4) {
        while (true) {
            std::cin >> genName1;
            std::cin >> genName2;
            loadGenomFromFile(genom1, genName1);
            loadGenomFromFile(genom2, genName2);
            numOfgens = 0;
            identicalGens = 0;
            if (genom1.size() > genom2.size()) biggestSizeGenom = genom1.size();
            else biggestSizeGenom = genom2.size();

            for (int iy = 0; iy < biggestSizeGenom; iy++) {
                for (int ix = 0; ix < CHROMOSOME_SIZE; ix++) {
                    numOfgens++;
                    if (genom1.size() > iy and genom2.size() > iy) {
                        if (genom1[iy][ix] == genom2[iy][ix]) {
                            std::cout << "#";
                            identicalGens++;
                        }
                        else {
                            std::cout << "-";
                        }
                    }
                    else {
                        std::cout << "-";
                    }
                }
                std::cout << "\n";
            }
            std::cout << "\n" << (double)(identicalGens) * 100 / (double)(numOfgens) << "% cовпадает \n";
        }
    }
    else {
        std::cout << "введите количество растений\n";
        std::cin >> iter;
        std::cout << "Процент просчитаных веток на этом слое | просчитаных слоёв | просчитаных растений\n";
        for (int i = 0; i < iter; i++) {
            genom.clear();
            if (mode == 1) {
                genom = std::vector<std::vector<unsigned char>>(rand() % 18 + 3, std::vector<unsigned char>(CHROMOSOME_SIZE));
                genomRand(genom);
                seedToRandTurn = rd();
            }
            else if (mode == 2) {
                loadGenomFromFile(genom1, genName1);
                loadGenomFromFile(genom2, genName2);
                crossingPlants(genom, genom1, genom2);
                seedToRandTurn = rd();
            }
            else if (mode == 3) {
                loadGenomFromFile(genom, genName1);
                loadSeedToRandTurn(genName1);
            }

            if (mode == 2 or mode == 3) {
                for (int i = 0; i < randy; i++) {
                    genom[rand() % genom.size()][rand() % CHROMOSOME_SIZE] = rand() % 256;
                }
                for (int i = 0; i < fluctuation; i++) {
                    genom[rand() % genom.size()][rand() % CHROMOSOME_SIZE] += rand() % 21 - 11;
                }
                if (chromosomeEdit) {
                    if (rand() % 5 == 0) {
                        if (rand() % 2 == 0) {
                            genom.erase(genom.begin() + rand() % genom.size());
                        }
                        else {
                            genom.push_back({});
                            for (unsigned int i = 0; i < CHROMOSOME_SIZE; i++) {
                                genom[genom.size() - 1].push_back(rand() % 256);
                            }
                        }
                    }
                }
            }

            for (int iy = 0; iy < genom.size(); iy++) {
                if (genom[iy][CHROMOSOME_BRANCHE1] > genom.size()) {
                    genom[iy][CHROMOSOME_BRANCHE1] = genom[iy][CHROMOSOME_BRANCHE1] % (genom.size() + 1);
                }
                if (genom[iy][CHROMOSOME_BRANCHE2] > genom.size()) {
                    genom[iy][CHROMOSOME_BRANCHE2] = genom[iy][CHROMOSOME_BRANCHE2] % (genom.size() + 1);
                }
                if (genom[iy][CHROMOSOME_BRANCHE3] > genom.size()) {
                    genom[iy][CHROMOSOME_BRANCHE3] = genom[iy][CHROMOSOME_BRANCHE3] % (genom.size() + 1);
                }
            }


            id = std::to_string(timeStart) + "_" + std::to_string(i);
            agents.clear();

            agents.push_back({});
            agents[0].push_back({});
            agents[0][0] = loadChromosome(genom, 0, X / 2, Y / 2 + Y / 4);
            startScreen(screen);

            int randp;
            saveGenom(genom, id);

            drawPlant(genom, agents, screen, i, iter);

            if (quality) {
                creatPNG(id, screen);
            }
            else {
                save_as_jpeg(screen, id, 10);
            }
        }
    }
}
