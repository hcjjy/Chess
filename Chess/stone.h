#ifndef STONE_H
#define STONE_H
#include<QString>

class Stone
{
public:
    Stone();
	Stone(int id);
    void init(int id);
	void rotate();
    ~Stone();
    QString getText();
public:
    enum TYPE{CHE,MA,XIANG,SHI,JIANG,PAO,BING};
public:
    int _row;
    int _col;
    int _id;
    bool _dead;
    bool _red;
    TYPE _type;

};

#endif // STONE_H
