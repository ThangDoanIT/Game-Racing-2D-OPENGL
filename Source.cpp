#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE  
#define _CRT_NONSTDC_NO_DEPRECATE

#include "Dependencies/freeglut/freeglut.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <windows.h>
#include <iostream>
#include <sys/timeb.h>
#include <sys/utime.h>
using namespace std;

//biến chạy
int i;
//điểm
int score = 0;
//số mạng còn lại
int collide = 3;

char buffer[10];

//biến lưu trữ các thông báo màn hình
char gameOver[] = "Game Over";
char scoreLabel[] = "Score";
char playAgainLabel[] = "Play again? Press y or n";
char lifeLabel[] = "Life";
char speedText[] = "Speed";
char kmh[] = "km/h";

//chinh speed, mấy biến đếm chu kì
int sleeptime, biSleepTime, biSleepTime1;
// toc do
float speed;
//vị trí của xe trong tọa độ Oxy, x = 200 và y = 50
int vehicleX = 200, vehicleY = 70;
// mảng chứa tọa độ xe địch
int ovehicleX[4], ovehicleY[4];
//vị trí của đường ray trong tọa độ 0xy, x = 250 và y = 4
int divx = 250, divx1 = 350, divy = 4, movd;
int posBi[3] = { 200, 300, 400 };
void update();
void init();
int enterOption();
int checkValue();
// Đếm giây
int getMiliCount()
{
    timeb tb;
    ftime(&tb);
    int ncount = tb.millitm + (tb.time & 0xfffff) * 1000;
    return ncount;

}
//Ngủ
void sleep(int sleeptime)
{
    int count = 0;
    int beginSleep = getMiliCount();
    while (getMiliCount() - beginSleep < sleeptime) {
        count++;
    }
}
// hàm vẽ text xác định vị trí của text muốn hiển thị
void drawText(char ch[], int xpos, int ypos)
{
    //đếm số kí tự của chuỗi
    int numofchar = strlen(ch);
    glLoadIdentity();
    glRasterPos2f(xpos, ypos);
    //dùng vòng lặp để hiển thị
    for (i = 0; i <= numofchar - 1; i++)
    {

        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ch[i]);
    }
}
// hàm vẽ text xác định vị trí của text muốn hiển thị
void drawTextNum(char ch[], int numtext, int xpos, int ypos)
{
    int len;
    int k;
    k = 0;
    len = numtext - strlen(ch);
    glLoadIdentity();
    glRasterPos2f(xpos, ypos);
    for (i = 0; i <= numtext - 1; i++)
    {
        if (i < len)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, '0');
        else
        {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ch[k]);
            k++;
        }

    }
}
// vị trí của xe kẻ địch
void ovpos()
{
    glClearColor(0, 0, 1, 0);
    ovehicleX[0] = vehicleX;
    ovehicleY[0] = 1000 - i * 160;

    for (i = 1; i < 4; i++)
    {
        if (rand() % 2 == 0) {
            ovehicleX[i] = 200;
        }
        else if (rand() % 2 == 1) {
            ovehicleX[i] = 400;
        }
        else {
            ovehicleX[i] = 300;
        }
        
        ovehicleY[i] = 1000 - i * 160;
    }
}
// vẽ đường
void drawRoad()// vẽ con đường
{
    glBegin(GL_QUADS);
    glColor3f(51.0/255.0, 51.0 / 255.0, 51.0 / 255.0);

    glVertex2f(150, 0);
    glVertex2f(150, 750);
    glVertex2f(450, 750);
    glVertex2f(450, 0);
    glEnd();
}
// vẽ đường chia cắt
void drawDivider()//vẽ đường ray màu đen 
{
    glLoadIdentity();
    // cho từng mảnh đường ray lần lượt tịnh tiến xuống theo trục Oy
    glTranslatef(0, movd, 0);
    //dùng vòng lặp for để vẽ lần lượt 10 vạch kẻ
    for (i = 1; i <= 15; i++)
    {
        glColor3f(1, 1, 1);
        glBegin(GL_QUADS);
        //divx = 250 , divy = 4
        glVertex2f(divx - 5, divy * 15 * i + 18);
        glVertex2f(divx - 5, divy * 15 * i - 18);
        glVertex2f(divx + 5, divy * 15 * i - 18);
        glVertex2f(divx + 5, divy * 15 * i + 18);
        glEnd();
    }
    for (i = 1; i <= 15; i++)
    {
        glColor3f(1, 1, 1);
        glBegin(GL_QUADS);
        //divx = 250 , divy = 4
        glVertex2f(divx1 - 5, divy * 15 * i + 18);
        glVertex2f(divx1 - 5, divy * 15 * i - 18);
        glVertex2f(divx1 + 5, divy * 15 * i - 18);
        glVertex2f(divx1 + 5, divy * 15 * i + 18);
        glEnd();
    }

    glLoadIdentity();
}
// vẽ xe
void drawVehicle()
{
    glPointSize(10.0);
    glBegin(GL_POINTS); // vẽ điểm
    glColor3f(0, 0, 0);
    glVertex2f(vehicleX - 25, vehicleY + 16);
    glVertex2f(vehicleX + 25, vehicleY + 16);
    glVertex2f(vehicleX - 25, vehicleY - 16);
    glVertex2f(vehicleX + 25, vehicleY - 16);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(245.0/255.0, 19.0 / 255.0, 10.0 / 255.0);
    glVertex2f(vehicleX - 25, vehicleY + 20);
    glVertex2f(vehicleX - 25, vehicleY - 20);
    glVertex2f(vehicleX + 25, vehicleY - 20);
    glVertex2f(vehicleX + 25, vehicleY + 20);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0, 0, 1);
    glVertex2f(vehicleX - 23, vehicleY + 20);
    glVertex2f(vehicleX - 19, vehicleY + 40);
    glVertex2f(vehicleX + 19, vehicleY + 40);
    glVertex2f(vehicleX + 23, vehicleY + 20);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0, 0, 1);
    glVertex2f(vehicleX - 23, vehicleY - 20);
    glVertex2f(vehicleX - 19, vehicleY - 35);
    glVertex2f(vehicleX + 19, vehicleY - 35);
    glVertex2f(vehicleX + 23, vehicleY - 20);
    glEnd();
}
// hàm vẽ xe địch , vẽ 1 lần 4 chiếc xe có thể tùy chỉnh được trong vòng for
void drawOVehicle()
{

    //vẽ 4 chiếc xe liên tục với các tọa độ khác nhau
    for (i = 0; i < 4; i++)
    {
        //vẽ thân xe
        glBegin(GL_QUADS);
        glColor3f(0.99609, 0.83984, 0);
        glVertex2f(ovehicleX[i] - 25, ovehicleY[i] + 20);
        glVertex2f(ovehicleX[i] - 25, ovehicleY[i] - 20);
        glVertex2f(ovehicleX[i] + 25, ovehicleY[i] - 20);
        glVertex2f(ovehicleX[i] + 25, ovehicleY[i] + 20);
        glEnd();

        // vẽ bánh xe
        glPointSize(10.0);
        glBegin(GL_POINTS);
        glColor3f(0, 0, 0);
        glVertex2f(ovehicleX[i] - 25, ovehicleY[i] + 16);
        glVertex2f(ovehicleX[i] + 25, ovehicleY[i] + 16);
        glVertex2f(ovehicleX[i] - 25, ovehicleY[i] - 16);
        glVertex2f(ovehicleX[i] + 25, ovehicleY[i] - 16);
        glEnd();

        // vẽ đầu xe 
        glBegin(GL_QUADS);// vẽ hình tứ giác
        glColor3f(0, 1, 0);
        //vẽ đầu xe với 4 điểm trong tọa độ Oxy
        glVertex2f(ovehicleX[i] - 23, ovehicleY[i] + 20);
        glVertex2f(ovehicleX[i] - 19, ovehicleY[i] + 40);
        glVertex2f(ovehicleX[i] + 19, ovehicleY[i] + 40);
        glVertex2f(ovehicleX[i] + 23, ovehicleY[i] + 20);
        glEnd();


        glBegin(GL_QUADS); // vẽ tứ giác
        glColor3f(0, 1, 0);
        //vẽ đuôi xe với 4 điểm trong tọa độ Oxy
        glVertex2f(ovehicleX[i] - 23, ovehicleY[i] - 20);
        glVertex2f(ovehicleX[i] - 19, ovehicleY[i] - 35);
        glVertex2f(ovehicleX[i] + 19, ovehicleY[i] - 35);
        glVertex2f(ovehicleX[i] + 23, ovehicleY[i] - 20);
        glEnd();

        //tốc độ của xe địch
        ovehicleY[i] = ovehicleY[i] - 5;

        /* điều kiện thua của game , vehicleY = 70 ;
        */
        if (ovehicleY[i] > vehicleY - 25 - 25 && ovehicleY[i] < vehicleY + 25 + 25 && ovehicleX[i] == vehicleX)
        {
            collide--;
            glutIdleFunc(NULL);
            ovpos();
            glutIdleFunc(update);
        }
        // game được làm trong gốc tọa độ I ( gốc có x dương và y dương) nên khi vị trí của ovehicle xuống dưới 0
        // sẽ tạo lại xe đó ở vị trí dương
        if (ovehicleY[i] < -50)
        {
            //random cho xe ở vị trí nào trong game hoặc là ở đối diện xe hoặc là ở bên kia xe tính theo trục Ox
            if (vehicleX <= 200)
            {
                ovehicleX[i] = 200;
            }
            else if (vehicleX > 200 && vehicleX < 400)
            {
                ovehicleX[i] = 300;
            }
            else {
                ovehicleX[i] = 400;
            }

            ovehicleY[i] = 600;
        }
    }
}
// nhập phím mũi tên trái phải để di chuyển xe
void Specialkey(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_LEFT:
        vehicleX -= 100;
        break;
    case GLUT_KEY_RIGHT:
        vehicleX += 100;
        break;
        // nhấn mũi tên lên để tăng tốc
    case GLUT_KEY_UP:
        sleeptime -= 0.5;
        break;
    }

    glutPostRedisplay();
}
// nhập phím y để tiếp tục chơi game và phím n để thoát
void Normalkey(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'y':
        score = 0;
        collide = 3;
        ovpos();
        glutIdleFunc(update);
        break;

    case 'n':
        exit(0);
    }
}
//Khởi tạo
void init()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //thay đổi góc nhìn 
    gluOrtho2D(0, 750, 0, 750);
    glMatrixMode(GL_MODELVIEW);
}
// sinh xe ngau nhien, noi nao có xe nơi đó có xe địch
void randomCar() {
    if (vehicleX <= 450) {
        ovehicleX[2] = 200;

    }
    else if (vehicleX > 450 && vehicleX < 550) {
        ovehicleX[1] = 300;
    }
    else {
        ovehicleX[0] = 400;
    }
}
// tịnh tiến qua lại của xe địch
void randomTranslateCar() {
    int i = rand() % 4;
    int j = rand() % 3;

    if (ovehicleX[i] != posBi[j] && ovehicleY[i] > 200) {
        ovehicleX[i] = posBi[j];
    }
}
// hàm Update
void update()
{
    glClear(GL_COLOR_BUFFER_BIT);
    drawRoad(); // vẽ đường
    drawDivider(); // vẽ đường ray
    drawVehicle(); // vẽ xe
    drawOVehicle(); // vẽ chướng ngại vật

     //limit fps
    int beginFrame = getMiliCount();
    int timeDiff = getMiliCount() - beginFrame;


    if (timeDiff < sleeptime)
    {
        sleep(sleeptime - timeDiff);
        speed = (float)(1000 / sleeptime);
    }

    if (biSleepTime == 100) {
        biSleepTime = 0;
        sleeptime -= 0.1;
    }

    biSleepTime += 1;

    //cho đường ray tịnh tiến theo trục Oy nếu giá trị xuống dưới 0 thì khởi tạo lại, giá trị của mod trong hàm
    // gltranslate() 

    // điều kiện để tái tạo đường ray cho đường ray di chuyển cho modv giảm dần theo trục Oy
    // nếu movd < -60 tức là đường ray xuống vị trí âm sẽ cho đường ray biến mất và tạo lại tại ví trí movd = 0
    movd = movd - 15;
    if (movd < -60)
    {
        movd = 0;
    }
    // giới hạn lề
    if (vehicleX < 150) {
        vehicleX = 200;
    }

    if (vehicleX > 500) {
        vehicleX = 400;
    }

    //update score
    score = score + 1;

    glColor3f(1, 1, 1);

    //text score
    drawText(scoreLabel, 530, 700);
    _itoa(score, buffer, 10);
    drawTextNum(buffer, 6, 600, 700);
    if (collide > 0) {
        //text mang song con lai
        drawText(lifeLabel, 530, 600);
        _itoa(collide, buffer, 10);
        drawTextNum(buffer, 1, 580, 600);
    }

    // text speed
    drawText(speedText, 530, 500);
    _itoa(speed, buffer, 10);
    drawTextNum(buffer, 3, 600, 500);
    drawText(kmh, 630, 500);
    //render hình ảnh
    glutSwapBuffers();

    // các điều kiện khi mạng = 0
    if (collide == 0)
    {
        //giá trị null để không xử lí các hàm
        glutIdleFunc(NULL);
        glColor3f(1, 1, 1);
        //text GAMEOVER
        drawText(gameOver, 530, 400);
        //Text playagain
        drawText(playAgainLabel, 530, 300);
        //Text mang con lai
        drawText(lifeLabel, 530, 600);
        _itoa(collide, buffer, 10);
        drawTextNum(buffer, 1, 580, 600);
        //reset lai toc do
        sleeptime = 35;

        glutSwapBuffers();
    }
    // nơi nào có xe nơi đó có địch
    randomCar();

    // xe địch tự tịnh tiến random
    if (biSleepTime1 == 40) {
        randomTranslateCar();
        biSleepTime1 = 0;
    }
    biSleepTime1++;
}
// hàm chạy game
int play(int argc, char** argv) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 500);
    glutCreateWindow("DRIFTER");
    ovpos();
    init();
    glutDisplayFunc(update);
    glutSpecialFunc(Specialkey);
    glutKeyboardFunc(Normalkey);
    glutIdleFunc(update);
    glutMainLoop();
    return 0;
}
//menu console
int menu(int argc, char** argv) {

    int x = 0;
    do {
        cout << "Chao mung moi nguoi den voi Thai Cong TV" << endl;
        cout << "Moi ban chon che do choi" << endl;
        cout << "1. De" << endl;
        cout << "2. Trung Binh" << endl;
        cout << "3. Kho" << endl;
        cout << "0. Thoat" << endl;

        x = checkValue();

        switch (x) {
        case 1:
            cout << "Ban chon che do De." << endl;
            Sleep(2000);
            sleeptime = 45;
            play(argc, argv);
            break;
        case 2:
            cout << "Ban chon che do Trung Binh." << endl;
            Sleep(2000);
            sleeptime = 35;
            play(argc, argv);
            break;
        case 3:
            cout << "Ban chon che do Kho." << endl;
            Sleep(2000);
            sleeptime = 25;
            play(argc, argv);
            break;
        case 0:
            cout << "Ban da thoat." << endl;
            break;
        default:
            break;
        }
        system("CLS");
    } while (x != 0);
    return 0;
}
// nhap muc do
int enterOption() {
    int x = 0;
    cout << "Nhap Phim: " << endl;
    cin >> x;
    return x;
}
// kiem tra gia tri nhap vao cua nguoi dung
int checkValue() {
    int x = 0;

    do {
        x = enterOption();
        if (x < 0 || x > 3) {
            cout << "Ban nhap sai roi. " << endl;
            cout << "Xin cam on." << endl;
        }
    } while (x < 0 || x > 3);
    return x;
}

int main(int argc, char** argv)
{
    menu(argc, argv);
    return 0;
}