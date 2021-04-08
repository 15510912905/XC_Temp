/**********************************************************
** Si(Xi)=ai*(X-Xi)^3+bi*(X-Xi)^2+ci*(X-Xi)+di
** 三次样条插值函数.C
** 进行自然边界，夹持边界，非扭结边界条件下的插值
** 
** 2012-05-25 将函数从原来的需要至少4个插值点才能计算
**            扩展成只需要2个插值点就可以完成计算
**            其他一些改变
** 2008-12-27 创建函数
**********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "Spline.h"

#define ABSOLUTE(p) ( ((p) > 0) ? (p) : -(p) )
#define SWAP(x, y, temp) (temp) = (y); (y) = (x); (x) = (temp);
#define PI (3.14159265)

typedef enum _condition
{
    NATURAL=0, // 自然边界
    CLAMPED, // 夹持边界
    NOTAKNOT // 非扭结边界
}CONDITION;

typedef struct _coefficient
{
    double a3;
    double b2;
    double c1;
    double d0;
}COEFFICIENT;

typedef struct _point
{
	COEFFICIENT *coe;    // 插值结果系数矩阵
    double *xCoordinate; // 插值点横坐标
    double *yCoordinate; // 插值点纵坐标
    double f0;           // 在夹持条件下的最左边点的导数值
    double fn;           // 在夹持条件下的最右边点的导数值
    int num;             // 插值点数
	CONDITION con;       // 边界条件
}POINT;

/* The tridiagonal matrix algorithm (TDMA) 高斯消元 */
/*
求解如下多项式的解
b[0]m[0] + c[0]m[1] = x[0]
a[1]m[1] + b[1]m[2] + c[1]m[3] = x[1]
a[2]m[2] + b[2]m[3] + c[2]m[4] = x[2]
.
.
.
a[n-3]m[n-4] + b[n-3]m[n-3] + c[n-3]m[n-2] = x[n-3]
a[n-2]m[n-3] + b[n-2]m[n-2] + c[n-2]m[n-1] = x[n-2]
a[n-1]m[n-2] + b[n-1]m[n-1] = x[n-1]
x[] : 输入时为等式结果
N   : 输入等式个数 必须N=sizeof(x[])-1
a[] : 输入a[i]系数
b[] : 输入b[i]系数
c[] : 输入c[i]系数
|b[i]| > |a[i]| + |c[i]|
*/
int tdma(double x[], const unsigned int N, const double a[], const double b[], double c[])
{
	unsigned int n;
	double m;

	if ( (ABSOLUTE(b[1]) <= ABSOLUTE(a[1]) + ABSOLUTE(c[1]))||(N<2)){
		return -1;
	}

	/* b[0]*x1+c[0]*x2=x[0] x1+(c[0]/b[0])*x2=x[0]/b[0] */
	c[0] = c[0] / b[0];    /* c0' */
	x[0] = x[0] / b[0];    /* x0' */

	/* 经过此循环后c0~c(n-1) 变为c0’~c(n-1)' x0~x(n-1) 变为x0’~x(n-1)' */
	for (n = 1; n < N; n++) {                      /* 数学公式推导所得 c1'~cn'导数值 d1'~dn'导数值 */
		m = 1.0f / (b[n] - a[n] * c[n - 1]);       /* 1/(bi-(c(i-1)'*ai)) */
		c[n] = c[n] * m;                           /* ci/(bi-(c(i-1)'*ai)) */
		x[n] = (x[n] - a[n] * x[n - 1]) * m;       /* (xi-(x(i-1)'*ai))/(bi-(c(i-1)'*ai)) */
	}

	/* 经过此函数后 x0~x(n-1)变为m0~m(n-1)的解 */
	for (n = N - 1; n-- > 0;){                    /* 逆序求出m[i] m[i] = x[i-1]'-c[i]'x[i+1] */
		x[n] = x[n] - c[n] * x[n + 1];
	}

	/* 此函数返回的解在x[i]中 x[]在此函数中有三层含义 第一次含义为x[i]输入结果值
	第二次含义为存储x[i]'的导数 第三次含义为存储得到的m[i]的解  */
	return 0;
}

/*
** 资源不足时函数返回 -1
** 插值点数小于2时，函数返回 -2
** 计算正确完成函数返回插值点的数量 n
*/
int splineA(POINT *point)
{
    double *x = (*point).xCoordinate, *y = (*point).yCoordinate;
    int n = (*point).num - 1;          /* 循环上限 */
    int i;                             /* 循环辅助变量 */
	COEFFICIENT *coe = (*point).coe;   /* 存储a3 b2 c1 d0 */
	CONDITION con = (*point).con;      /* 选择边界条件 */
    double *h, *d;                     /* h:为步长 d为微分 */
    double *a, *b, *c, *f, *m;         /* a b c 为方程系数 f 为方程结果 m为待求未知数(二次微分值)*/
    double temp;

    if (n < 1) {
		return -2;
	}

    h = (double *)malloc( (6 * n + 4) * sizeof(double) ); /* 0,       1,...,(n-1)          */
    if (h == NULL) {
		return -1;
	}
    d = h + n;                                            /* 0,       1,...,(n-1)          */
    a = d + n;                                            /* 特别使用,1,...,(n-1),       n */
    b = a + (n + 1);                                      /*        0,1,...,(n-1),       n */
    c = b + (n + 1);                                      /*        0,1,...,(n-1),特别使用 */
    f = c + (n + 1);                                      /*        0,1,...,(n-1),       n */
    m = b;

    /* 计算 h[] d[] */
    for (i = 0; i < n; i++)
    {
        h[i] = x[i + 1] - x[i];                          /* 存储步长及差值 */
        d[i] = (y[i + 1] - y[i]) / h[i];                 /* 存储微分 */
        /* printf("%f\t%f\n", h[i], d[i]); */
    }
    /**********************
    ** 初始化系数增广矩阵
    **********************/
    a[0] = (*point).f0;
    c[n] = (*point).fn;
    /* 计算 a[] b[] c[] f[] */
    switch(con)
    {
		case NATURAL:
			f[0] = 0;
			f[n] = 0;
			a[0] = 0;
			a[n] = 0;
			c[0] = 0;
			c[n] = 0;
			b[0] = 1;
			b[n] = 1;
			break;

		case CLAMPED:
			f[0] = 6 * (d[0] - a[0]);
			f[n] = 6 * (c[n] - d[n - 1]);
			a[0] = 0;
			c[n] = 0;
			c[0] = h[0];
			a[n] = h[n - 1];
			b[0] = 2 * h[0];
			b[n] = 2 * h[n - 1];
			break;

		case NOTAKNOT:
			f[0] = 0;
			f[n] = 0;
			a[0] = h[0];
			c[n] = h[n - 1];
			c[0] = -(h[0] + h[1]);
			a[n] = -(h[n - 2] + h[n - 1]);
			b[0] = h[1];
			b[n] = h[n - 2];
			break;
    }
	
	/* 构建矩阵方程 三对角矩阵 */
    for (i = 1; i < n; i++)
    {
        a[i] = h[i - 1];
        b[i] = 2 * (h[i - 1] + h[i]);
        c[i] = h[i];
        f[i] = 6 * (d[i] - d[i - 1]);
    }
    /* for (i = 0; i < n+1; i++){   printf("%f\n", c[i]); }*/

    /*************
    ** 高斯消元
    *************/
    if (n > 2)
    {
        /* 第0行到第(n-3)行的消元 */
        for(i = 0; i <= n - 3; i++)
        {
            /* 选主元 */
			if (ABSOLUTE(a[i + 1]) > ABSOLUTE(b[i]))
            {
                SWAP(a[i + 1], b[i], temp);
                SWAP(b[i + 1], c[i], temp);
                SWAP(c[i + 1], a[i], temp);
                SWAP(f[i + 1], f[i], temp);
            }
            temp = a[i + 1] / b[i];
            a[i + 1] = 0;
            b[i + 1] = b[i + 1] - temp * c[i];
            c[i + 1] = c[i + 1] - temp * a[i];
            f[i + 1] = f[i + 1] - temp * f[i];
        }
    }
    if (n >= 2)
    {
        /* 最后3行的消元 */
        /* 第(n-2)行的消元 */
        /* 选主元 */
		if (ABSOLUTE(a[n - 1]) > ABSOLUTE(b[n - 2]))
        {
            SWAP(a[n - 1], b[n - 2], temp);
            SWAP(b[n - 1], c[n - 2], temp);
            SWAP(c[n - 1], a[n - 2], temp);
            SWAP(f[n - 1], f[n - 2], temp);
        }
        /* 选主元 */
		if (ABSOLUTE(c[n]) > ABSOLUTE(b[n - 2]))
        {
            SWAP(c[n], b[n - 2], temp);
            SWAP(a[n], c[n - 2], temp);
            SWAP(b[n], a[n - 2], temp);
            SWAP(f[n], f[n - 2], temp);
        }
        /* 消元 */
        temp = a[n - 1] / b[n - 2];
        a[n - 1] = 0;
        b[n - 1] = b[n - 1] - temp * c[n - 2];
        c[n - 1] = c[n - 1] - temp * a[n - 2];
        f[n - 1] = f[n - 1] - temp * f[n - 2];
        /* 消元 */
        temp = c[n] / b[n - 2];
        c[n] = 0;
        a[n] = a[n] - temp * c[n - 2];
        b[n] = b[n] - temp * a[n - 2];
        f[n] = f[n] - temp * f[n - 2];
    }
    /* 最后2行的消元 */
    /* 第(n-1)行的消元 */
    /* 选主元 */
	if (ABSOLUTE(a[n]) > ABSOLUTE(b[n - 1]))
    {
        SWAP(a[n], b[n - 1], temp);
        SWAP(b[n], c[n - 1], temp);
        SWAP(f[n], f[n - 1], temp);
    }
    /* 消元 */
    temp = a[n] / b[n-1];
    a[n] = 0;
    b[n] = b[n] - temp * c[n - 1];
    f[n] = f[n] - temp * f[n - 1];

    /******************
    ** 回代求解 m[] = f[] / b[] 
    ******************/
    m[n] = f[n] / b[n];
    m[n - 1] = (f[n - 1] - c[n - 1] * m[n]) / b[n-1];
    for (i = n - 2; i >= 0; i--)
    {
        m[i] = ( f[i] - (m[i + 2] * a[i] + m[i + 1] * c[i]) ) / b[i];
    }
    /* for (i = 0; i < n+1; i++)   printf("%f\n", m[i]); */

    /***********************
    ** 计算插值函数所有参数
    ***********************/
    for (i = 0; i < n; i++)                                               /* 数学公式化简而得 */
    {
        coe[i].a3 = (m[i + 1] - m[i]) / (6 * h[i]);                       /* */
        coe[i].b2 = m[i] / 2;                                             /* */
        coe[i].c1 = d[i] - (h[i] / 6) * (2 * m[i] + m[i + 1]);			  /* */
        coe[i].d0 = y[i];                                                 /* d0 = y[i] */
    }
    free(h);
    // 计算正确完成返回插值点的数量
    return n + 1;
}

/*
** 资源不足时函数返回 -1
** 插值点数小于2时，函数返回 -2
** 计算正确完成函数返回插值点的数量 n
*/
int splineB(POINT *point)
{
	double *x = (*point).xCoordinate, *y = (*point).yCoordinate;
	int n = (*point).num - 1;          /* 循环上限 */
	int i;                             /* 循环辅助变量 */
	COEFFICIENT *coe = (*point).coe;   /* 存储a3 b2 c1 d0 */
	CONDITION con = (*point).con;      /* 选择边界条件 */
	double *h, *d;                     /* h:为步长 d为微分 */
	double *a, *b, *c, *f, *m;         /* a b c 为方程系数 f 为方程结果 m为待求未知数(二次微分值)*/

	if (n < 1) {
		return -2;
	}

	h = (double *)malloc((6 * n + 4) * sizeof(double)); /* 0,       1,...,(n-1)          */
	if (h == NULL) {
		return -1;
	}
	d = h + n;                                            /* 0,       1,...,(n-1)          */
	a = d + n;                                            /* 特别使用,1,...,(n-1),       n */
	b = a + (n + 1);                                      /*        0,1,...,(n-1),       n */
	c = b + (n + 1);                                      /*        0,1,...,(n-1),特别使用 */
	f = c + (n + 1);                                      /*        0,1,...,(n-1),       n */
	m = b;

	/* 计算 h[] d[] */
	for (i = 0; i < n; i++)
	{
		h[i] = x[i + 1] - x[i];                          /* 存储步长及差值 */
		d[i] = (y[i + 1] - y[i]) / h[i];                 /* 存储微分 */
		/* printf("%f\t%f\n", h[i], d[i]); */
	}
	/**********************
	** 初始化系数增广矩阵
	**********************/
	a[0] = (*point).f0;
	c[n] = (*point).fn;
	/* 计算 a[] b[] c[] f[] */
	switch (con)
	{
		case NATURAL:
			f[0] = 0;
			f[n] = 0;
			a[0] = 0;
			a[n] = 0;
			c[0] = 0;
			c[n] = 0;
			b[0] = 1;
			b[n] = 1;
			break;

		case CLAMPED:
			f[0] = 6 * (d[0] - a[0]);
			f[n] = 6 * (c[n] - d[n - 1]);
			a[0] = 0;
			c[n] = 0;
			c[0] = h[0];
			a[n] = h[n - 1];
			b[0] = 2 * h[0];
			b[n] = 2 * h[n - 1];
			break;

		case NOTAKNOT:
			f[0] = 0;
			f[n] = 0;
			a[0] = h[0];
			c[n] = h[n - 1];
			c[0] = -(h[0] + h[1]);
			a[n] = -(h[n - 2] + h[n - 1]);
			b[0] = h[1];
			b[n] = h[n - 2];
			break;
	}

	/* 构建矩阵方程 三对角矩阵 */
	for (i = 1; i < n; i++)
	{
		a[i] = h[i - 1];
		b[i] = 2 * (h[i - 1] + h[i]);
		c[i] = h[i];
		f[i] = 6 * (d[i] - d[i - 1]);
	}
	/* for (i = 0; i < n+1; i++){   printf("%f\n", c[i]); }*/

	
	//   /*************
	//   ** 高斯消元
	//   *************/
	if (tdma(f, n, a, b, c)<0){ 
		free(h);
		return -1;
	}
	memcpy( m, f, (n)*sizeof(double) );

	/***********************
	** 计算插值函数所有参数
	***********************/
	for (i = 0; i < n; i++)                                               /* 数学公式化简而得 */
	{
		coe[i].a3 = (m[i + 1] - m[i]) / (6 * h[i]);                       /* */
		coe[i].b2 = m[i] / 2;                                             /* */
		coe[i].c1 = d[i] - (h[i] / 6) * (2 * m[i] + m[i + 1]);			  /* */
		coe[i].d0 = y[i];                                                 /* d0 = y[i] */
	}
	free(h);
	// 计算正确完成返回插值点的数量
	return n + 1;
}

int iSplineDisCoe( COEFFICIENT* pCoe,int iSize )
{
	int i;

	printf("a:\t\tb:\t\tc:\t\td:\t\t\n");
	for (i = 0; i < iSize - 1; i++){
		printf("%f\t%f\t%f\t%f\n", pCoe[i].a3, pCoe[i].b2, pCoe[i].c1, pCoe[i].d0);
	}

	printf("\n");

	return i;
}

int iGetCurve(double* pCoe,int iSize, double dValue)
{
	int i;

	if ((pCoe[0] > dValue) ){
		return 0;
	}
	if ((pCoe[iSize-1] < dValue)){
		return iSize - 2;
	}
	for (i = 0; i < iSize-1; i++){
		if ((pCoe[i] <= dValue) && (pCoe[i+1] >= dValue))
		{
			return i;
		}
	}

	return -1;
}


double UnivariateCubiceQuationSolute(double a, double b, double c, double d, double num){
	double x, x0, f, f1;
	int cnt = 0;

	x = num;

	do{
		x0 = x;
		f = ((a*x0 + b)*x0 + c)*x0 + d;
		f1 = (3 * a*x0 + 2 * b)*x0 + c;
		x = x0 - f / f1;
		cnt++;
	} while ((fabs(x - x0) > 1e-12) && (cnt<10000));//while ((fabs(x - x0) > 1e-6)&&(cnt<10000)); (fabs(x - x0)

	printf("the value is %.14f %d\r\n", x, cnt );

	if (cnt > 10000){
		return 0.01;
	}
	else{
		return (x);
	}
}

/*
a 三次系数
b 二次系数
c 一次系数
d 常系数
*/
double dichotomy(double a, double b, double c, double d, double x1, double x2)
{
	//float a, b, c, d;
	//float x1, x2;
	double x0, fx1, fx2, fx0;

	//需满足f(x1)与f(x2)异号，且x1 < x2
	do{
		//输入求根范围
		//printf("input x1,x2:\r\n");
		//scanf_s("%f,%f", &x1, &x2);

		fx1 = ((a * x1 + b) * x1 + c) * x1 + d;
		fx2 = ((a * x2 + b) * x2 + c) * x2 + d;
	} while (fx1 * fx2 >= 0);

	do{
		x0 = (x1 + x2) / 2;
		fx0 = ((a * x0 + b) * x0 + c) * x0 + d;

		if ((fx0 * fx1) < 0){
			x2 = x0;
			fx2 = fx0;
		}
		else {
			x1 = x0;
			fx1 = fx0;
		}
	} while (fabs(fx0) >= 1e-6);//求根精度

	printf("the value is %.14f \r\n", x0);

	return x0;
}

//求根函数
double f(double a, double b, double c, double d, double x)
{
	double y;

	y = ((a * x + b) * x + c) * x + d;

	return y;
}

/* 求解与x轴交点 */
double xpoint(double a, double b, double c, double d, double x1, double x2)
{
	double y;

	y = (x1 * f(a, b, c, d, x2) - x2 * f(a, b, c, d, x1)) / (f(a, b, c, d, x2) - f(a, b, c, d, x1));

	return y;
}

/*
[ x1, x2 ]为给定的求根区间
需满足f(x1)与f(x2)异号
*/
double UnivariateCubiceQuation(double a, double b, double c, double d, double x1, double x2)
{
	//int i;
	double x=0;
	double y=0, y1=0;

	y1 = f(a, b, c, d, x1);

	do{
		x = xpoint(a, b, c, d, x1, x2);//求解与x轴交点
		y = f(a, b, c, d, x);
		if (y * y1 > 0){
			x1 = x;
			y1 = y;
		}
		else {
			x2 = x;
		}
	} while (fabs(y) >= 1e-6);//计算精度

	printf("the value is %.14f\r\n", x); 

	return x;
}

int UnivariateCubiceQuationShengjin(double a3, double b2, double c1, double d0, double *x1, double *x2, double *x3)
{
	double A = 0, B = 0, C = 0, ABC = 0;

	if (0 == (a3)){
		return -1;
	}

	A = pow(ABSOLUTE(b2), 2) - 3 * (a3)*(c1);
	B = (b2)*(c1) - 9 * (a3)*(d0);
	C = pow(ABSOLUTE((c1)), 2) - 3 * (b2)*(d0);
	ABC = pow(ABSOLUTE(B), 2) - 4 * A*C;

	if ((fabs(A)<1e-6) && (fabs(B)<1e-6)){
		*x1 = -((c1) / (b2));	
		*x2 = *x1;
		*x3 = *x1;
	}
	else if (0 == fabs(ABC)){//(1e-6 > fabs(ABC)){
		double K;

		K = B / A;
		*x1 = (-((b2) / (a3))) + K;
		*x2 = -(K / 2);
		*x3 = -(K / 2);
	}
	else if (1e-6 < ABC){
		double Y1, Y2;
		double RY1, RY2;

		Y1 = A*(b2) + 3 * (a3)*(((-B + pow(ABSOLUTE(ABC), (double)(1.0 / 2.0))) / 2));
		Y2 = A*(b2) + 3 * (a3)*(((-B - pow(ABSOLUTE(ABC), (double)(1.0 / 2.0))) / 2));

		if (Y1 < 0){
			RY1 = pow(ABSOLUTE(Y1), (double)(1.0 / 3.0));
			RY1 = -RY1;
		}
		else{
			RY1 = pow(Y1, (double)(1.0 / 3.0));
		}

		if (Y2 < 0){
			RY2 = pow(ABSOLUTE(Y2), (double)(1.0 / 3.0));
			RY2 = -RY2;
		}
		else{
			RY2 = pow(Y2, (double)(1.0 / 3.0));
		}

		*x1 = (-(b2) - (RY1 + RY2)) / 3 * (a3);

		*x2 = 0;
		*x3 = 0;
		//*x2 = (-(b2)+(1.0 / 2.0)*(RY1 + RY2) + (pow(3, 1.0 / 2.0)/2*(RY1-RY2))) / 3 * (a3);
		//*x3 = (-(b2)+(1.0 / 2.0)*(RY1 + RY2) - (pow(3, 1.0 / 2.0) / 2 * (RY1 - RY2))) / 3 * (a3);
	}
	else if (1e-6 > ABC){
		double r = 0;
		double T = 0;

		if (A < 1e-6){
			T = (2 * A*(b2) - 3 * (a3)*B) / (2 * -pow(ABSOLUTE(A), 3.0 / 2.0));
		}
		else{
			T = (2 * A*(b2) - 3 * (a3)*B) / (2 * pow(ABSOLUTE(A), 3.0 / 2.0));
		}

		r = acos(T);

		*x1 = (-(b2) - 2 * pow(ABSOLUTE(A), 1 / 2.0)*cos(r / 3.0)) / (3 * (a3));  //
		*x2 = (-(b2) + pow(ABSOLUTE(A), 1.0 / 2.0)*(cos(r / 3.0) + pow(3, 1.0 / 2.0)*sin(r / 3.0))) / (3 * (a3));  //
		*x3 = (-(b2) + pow(ABSOLUTE(A), 1.0 / 2.0)*(cos(r / 3.0) - pow(3, 1.0 / 2.0)*sin(r / 3.0))) / (3 * (a3));  //
	}
	else{
		return -1;
	}
	return 0;
}


int ShengjinA(RealNum rA, RealNum rB, RealNum rC, RealNum rD, RealNum* rX1, RealNum* rX2, RealNum* rX3)
{
	RealNum fDA = rB * rB - 3.0f * rA * rC;
	RealNum fDB = rB * rC - 9.0f * rA * rD;
	RealNum fDC = rC * rC - 3.0f * rB * rD;

	RealNum rTempA = ABSZERO;
	RealNum rTempB = ABSZERO;
	RealNum fDelta = fDB * fDB - 4.0f * fDA * fDC;
	if ((ISZERO(fDA) && ISZERO(fDB)))
	{
		if (!ISZERO(rA))
		{
			*rX1 = -1.0f * rB / (3.0f * rA);
		}
		else if (!ISZERO(rB))
		{
			*rX1 = -1.0f * rC / rB;
		}
		else if (!ISZERO(rC))
		{
			*rX1 = -1.0f * rD / rC;
		}
		else// if (!ISZERO(rD))
		{
			return 0;
		}
		*rX2 = *rX1;
		*rX3 = *rX1;
		return 1;
	}

	if (GREATERTHANZERO(fDelta))
	{
		double temp1, temp2, temp3;

		RealNum rY1 = fDA * rB + 3.0 * rA * (0.5f * (-1.0 * fDB + sqrt(fDelta)));
		RealNum rY2 = fDA * rB + 3.0 * rA * (0.5f * (-1.0 * fDB - sqrt(fDelta)));
		if (GREATERTHANZERO(rY1))
		{
			rTempA = pow(rY1, 1.0 / 3.0);
		}
		else
		{
			rTempA = -1.0 * pow(-1.0 * rY1, 1.0 / 3.0);
		}
		if (GREATERTHANZERO(rY2))
		{
			rTempB = pow(rY2, 1.0 / 3.0);
		}
		else
		{
			rTempB = -1.0 * pow(-1.0 * rY2, 1.0 / 3.0);
		}

		temp1 = -1.0 * (rB + rTempA + rTempB);
		temp2 = (3.0 * rA);
		temp3 = temp1 / temp2;

		*rX1 = -1.0 * (rB + rTempA + rTempB) * (1.0 / (3.0 * rA));
		Complex cX2;
		cX2.real = (1.0 / (6.0 * rA)) * (-2.0 * rB + rTempA + rTempB);
		cX2.image = (1.0 / (6.0 * rA)) * sqrtf(3.0) * (rTempA - rTempB);
		Complex cX3;
		cX3.real = cX2.real;
		cX3.image = -1.0f * cX2.image;

		return 1;
	}
	else if (ISZERO(fDelta))
	{
		if (ISZERO(fDA))
		{
			return 0;
		}
		RealNum rK = fDB / fDA;
		*rX1 = -1.0 * rB / rA + rK;
		*rX2 = -0.5 * rK;
		*rX3 = *rX2;
		return 2;
	}
	else
	{
		if (LESSTHANZERO(fDA))
		{
			return 0;
		}
		if (GREATERTHANZERO(fDA))
		{
			rTempA = pow(fDA, 3.0);
		}
		else
		{
			rTempA = -1.0 * pow(-1.0f * fDA, 3.0);
		}
		RealNum rT = (2.0 * fDA * rB - 3.0 * rA * fDB) / (2.0 * sqrt(rTempA));
		if ( !ISBETWEEN(rT, -1.0, 1.0) )
		{
			return 0;
		}
		RealNum rTheta = acos(rT);
		*rX1 = -1.0f * (rB + 2.0 * sqrt(fDA) * cos(rTheta / 3.0)) * (1.0f / (3.0f * rA));
		*rX2 = (-1.0f * rB + sqrt(fDA) * (cos(rTheta / 3.0) + sqrt(3.0) * sin(rTheta / 3.0))) * (1.0 / (3.0 * rA));
		*rX3 = (-1.0f * rB + sqrt(fDA) * (cos(rTheta / 3.0) - sqrt(3.0) * sin(rTheta / 3.0))) * (1.0 / (3.0 * rA));
		return 3;
	}

	return 0;
}

#if 1
void main()
{
	/* x[]内不能出现重复数据 */
	//double x[] = { 0, 0.5, 0.75, 1.25, 2.5,  5, 7.5, 10, 15,
	//              20,  25,   30,   35,  40, 45,  50, 55, 60,
	//              65,  70,   75,   80,  85, 90,  95, 100};
	//double y[] = {     0, 0.6107, 0.7424, 0.9470, 1.3074, 1.7773,    2.1,
	//              2.3414, 2.6726, 2.8688, 2.9706, 3.0009, 2.9743, 2.9015,
	//              2.7904, 2.6470, 2.4762, 2.2817, 2.0662, 1.8320, 1.5802,
	//              1.3116, 1.0263, 0.7239, 0.4033, 0.0630};

	double* x = NULL;
	double* y = NULL;
	double dInX = 0;
	double dInY = 0;
	double A=0,B = 0,C=0,ABC=0;
	int iCurve = 0;
	int iBoundaryConditions = 0;

	int n;
	COEFFICIENT *pCoefficient;
	POINT p;
	int i;

	//double recl = 0;
	//recl = pow(-4, 1/2.0);

	printf_s("Input Sample Size:\n");
	scanf_s("%d", &n);

	x = (double *)malloc((n)* sizeof(double));
	if (NULL == x){
		return;
	}
	y = (double *)malloc((n)* sizeof(double));
	if (NULL == y){
		return;
	}
	printf_s("Input %dx:\n", n);
	for (i = 0; i < n; i++){
		printf_s("%d\t", i + 1);
		scanf_s("%lf", (x + i));
	}
	printf_s("Input %dy:\n", n);
	for (i = 0; i < n; i++){
		printf_s("%d\t", i + 1);
		scanf_s("%lf", (y + i));
	}

	pCoefficient = (COEFFICIENT *)malloc((n - 1) * sizeof(COEFFICIENT));
	if (NULL == pCoefficient){
		return;
	}
	p.coe = pCoefficient;
	p.xCoordinate = x;
	p.yCoordinate = y;
	/* 下面两个参数f0和fn只在夹持条件下有效，其他条件下这两个值会被忽略 */
	p.f0 = 0;// 在夹持条件下的左边点的导数值
	p.fn = 0;// 在夹持条件下的右边点的导数值
	p.num = n;

	do{
		printf_s("NATURAL:0 CLAMPED:1 NOTAKNOT:2 \n");
		scanf_s("%d", &iBoundaryConditions);
	} while (iBoundaryConditions >= 3);

	p.con = iBoundaryConditions;//NATURAL CLAMPED NOTAKNOT ;
#if 1
	splineA(&p);
#else
	splineB(&p);
#endif

	iSplineDisCoe(pCoefficient, n);

	//while (1){
	//	printf_s("Input X:\n");
	//	scanf_s("%lf", &dInX);
	//	iCurve = iGetCurve(x, n, dInX);
	//	if (iCurve >= 0){
	//		printf_s("Si(Xi) = ai*(X - Xi) ^ 3 + bi*(X - Xi) ^ 2 + ci*(X - Xi) + di \n");
	//		printf_s("X:%lf\ti:%d\tXi:%lf\ta3:%lf\tb2:%lf\tc1:%lf\td0:%lf \n", dInX, iCurve, x[iCurve], ((pCoefficient + iCurve)->a3), ((pCoefficient + iCurve)->b2), ((pCoefficient + iCurve)->c1), ((pCoefficient + iCurve)->d0));
	//		dInY = ((pCoefficient + iCurve)->a3)*pow((dInX - x[iCurve]), 3) + ((pCoefficient + iCurve)->b2)*pow((dInX - x[iCurve]), 2) + ((pCoefficient + iCurve)->c1)*(dInX - x[iCurve]) + ((pCoefficient + iCurve)->d0);
	//		printf_s("Output Y:%lf\n", dInY);
	//	}
	//	else{
	//		printf_s("again ");//
	//	}
	//}

	while (1){
		double cal_d0 = 0;
		double Xx1, Xx2, Xx3;

		printf_s("Input Y:\n");
		scanf_s("%lf", &dInY);
		iCurve = iGetCurve(y, n, dInY);
		if (iCurve >= 0){
			printf_s("Si(Xi) = Y = ai*(X - Xi) ^ 3 + bi*(X - Xi) ^ 2 + ci*(X - Xi) + di \n");
			printf_s("Y:%lf\ti:%d\tXi:%lf\tX(i+1):%lf\ta3:%lf\tb2:%lf\tc1:%lf\td0:%lf\tdt:%lf \n", dInY, iCurve, x[iCurve],  x[iCurve+1],((pCoefficient + iCurve)->a3),
				      ((pCoefficient + iCurve)->b2), ((pCoefficient + iCurve)->c1), ((pCoefficient + iCurve)->d0), (((pCoefficient + iCurve)->d0) - dInY));

			//dInX = UnivariateCubiceQuationSolute(((pCoefficient + iCurve)->a3), ((pCoefficient + iCurve)->b2), ((pCoefficient + iCurve)->c1), (((pCoefficient + iCurve)->d0) - dInY), x[iCurve + 1]);
			//printf_s("A Output X:%lf Y:%lf \n", dInX, dInY);
			//dInX = x[iCurve] + dInX;	
			//dInY = ((pCoefficient + iCurve)->a3)*pow((dInX - x[iCurve]), 3) + ((pCoefficient + iCurve)->b2)*pow((dInX - x[iCurve]), 2) + ((pCoefficient + iCurve)->c1)*(dInX - x[iCurve]) + ((pCoefficient + iCurve)->d0);
			//printf_s("B Output X:%lf Y:%lf \n", dInX, dInY);

			cal_d0 = (((pCoefficient + iCurve)->d0) - dInY);
			if ( 0==((pCoefficient + iCurve)->a3) ){
				printf_s("a==0 \n");
				continue;
			}

			ShengjinA(((pCoefficient + iCurve)->a3), ((pCoefficient + iCurve)->b2), ((pCoefficient + iCurve)->c1), cal_d0, &Xx1, &Xx2, &Xx3);
			printf_s("Xx1:%.12lf X1:%.12lf Xx2:%.12lf X2:%.12lf Xx3:%.12lf X3:%.12lf \n", Xx1, x[iCurve] + Xx1, Xx2, x[iCurve] + Xx2, Xx3, x[iCurve] + Xx3);
	
/*			A = pow(ABSOLUTE(((pCoefficient + iCurve)->b2)), 2) - 3 * ((pCoefficient + iCurve)->a3)*((pCoefficient + iCurve)->c1);
			B = ((pCoefficient + iCurve)->b2)*((pCoefficient + iCurve)->c1) - 9 * ((pCoefficient + iCurve)->a3)*(((pCoefficient + iCurve)->d0) - dInY);
			C = pow(ABSOLUTE(((pCoefficient + iCurve)->c1)), 2) - 3 * ((pCoefficient + iCurve)->b2)*(((pCoefficient + iCurve)->d0) - dInY);
			ABC = pow(ABSOLUTE(B), 2) - 4 * A*C;	*/	

			
			A = pow((double)fabs(((pCoefficient + iCurve)->b2)), 2) - 3 * ((pCoefficient + iCurve)->a3)*((pCoefficient + iCurve)->c1);
			B = ((pCoefficient + iCurve)->b2)*((pCoefficient + iCurve)->c1) - 9 * ((pCoefficient + iCurve)->a3)*(cal_d0);
			C = pow((double)fabs(((pCoefficient + iCurve)->c1)), 2) - 3 * ((pCoefficient + iCurve)->b2)*(cal_d0);
			ABC = pow((double)fabs(B), 2) - 4 * A*C;

			printf_s("A:%.12lf B:%.12lf C:%.12lf ABC:%.12lf \n", A, B,C,ABC );

			if ((fabs(A)<1e-12)&&(fabs(B)<1e-12)){
				dInX = -(((pCoefficient + iCurve)->c1) / ((pCoefficient + iCurve)->b2));
				printf_s("A Output X:%lf Y:%lf \n", dInX, dInY);
				dInX = x[iCurve] + dInX;
				dInY = ((pCoefficient + iCurve)->a3)*pow((dInX - x[iCurve]), 3) + ((pCoefficient + iCurve)->b2)*pow((dInX - x[iCurve]), 2) + ((pCoefficient + iCurve)->c1)*(dInX - x[iCurve]) + ((pCoefficient + iCurve)->d0);
				printf_s("B Output X:%lf Y:%lf \n", dInX, dInY);
			}
			else if (1e-12 > fabs(ABC)){  //(0== fabs(ABC)){ //(1e-12 > fabs(ABC)){ 
				double K;

				if (1e-12 > fabs(A)){
					dInX = -(((pCoefficient + iCurve)->c1) / ((pCoefficient + iCurve)->b2));
					printf_s("A Output X:%lf Y:%lf \n", dInX, dInY);
					dInX = x[iCurve] + dInX;
					dInY = ((pCoefficient + iCurve)->a3)*pow((dInX - x[iCurve]), 3) + ((pCoefficient + iCurve)->b2)*pow((dInX - x[iCurve]), 2) + ((pCoefficient + iCurve)->c1)*(dInX - x[iCurve]) + ((pCoefficient + iCurve)->d0);
					printf_s("B Output X:%lf Y:%lf \n", dInX, dInY);
				}
				else if ( (1e-12 < fabs(B)) ){
					K = B / A;
					dInX = -((pCoefficient + iCurve)->b2) / ((pCoefficient + iCurve)->a3);
					dInX = (-((pCoefficient + iCurve)->b2) / ((pCoefficient + iCurve)->a3)) + K;
					printf_s("A Output X:%lf Y:%lf \n", dInX, dInY);
					dInX = x[iCurve] + dInX;
					dInY = ((pCoefficient + iCurve)->a3)*pow((dInX - x[iCurve]), 3) + ((pCoefficient + iCurve)->b2)*pow((dInX - x[iCurve]), 2) + ((pCoefficient + iCurve)->c1)*(dInX - x[iCurve]) + ((pCoefficient + iCurve)->d0);
					printf_s("B Output X:%lf Y:%lf \n", dInX, dInY);

					dInX = -(K / 2);
					printf_s("C Output X:%lf Y:%lf \n", dInX, dInY);
					dInX = x[iCurve] + dInX;
					dInY = ((pCoefficient + iCurve)->a3)*pow((dInX - x[iCurve]), 3) + ((pCoefficient + iCurve)->b2)*pow((dInX - x[iCurve]), 2) + ((pCoefficient + iCurve)->c1)*(dInX - x[iCurve]) + ((pCoefficient + iCurve)->d0);
					printf_s("D Output X:%lf Y:%lf \n", dInX, dInY);
				}
			}
			else if (1e-12 < ABC){
				double Y1, Y2;
				double RY1, RY2;
				double temp1, temp2, temp3;

				Y1 = A*((pCoefficient + iCurve)->b2) + 3.0 * ((pCoefficient + iCurve)->a3)*((-B + pow((double)fabs(ABC), (double)(1.0 / 2.0))) / 2.0);
				Y2 = A*((pCoefficient + iCurve)->b2) + 3.0 * ((pCoefficient + iCurve)->a3)*((-B - pow((double)fabs(ABC), (double)(1.0 / 2.0))) / 2.0);

				RY1 = (Y1 / fabs(Y1))*pow((double)fabs(Y1), (double)(1.0 / 3.0));
				RY2 = (Y2 / fabs(Y2))*pow((double)fabs(Y2), (double)(1.0 / 3.0));
		
				//*rX1 = -1.0 * (rB + rTempA + rTempB) * (1.0 / (3.0 * rA));
				temp1 = (-1.0*(((pCoefficient + iCurve)->b2) + RY1 + RY2));
				temp2 = 3.0 * ((pCoefficient + iCurve)->a3);
				temp3 = temp1 / temp2;
				dInX = (-1.0*(((pCoefficient + iCurve)->b2) + RY1 +  RY2)) / (3.0 * ((pCoefficient + iCurve)->a3));
				dInX = x[iCurve] + dInX;
				printf_s("A Output X:%lf Y:%lf \n", dInX, dInY);

				dInY = ((pCoefficient + iCurve)->a3)*pow((dInX - x[iCurve]), 3) + ((pCoefficient + iCurve)->b2)*pow((dInX - x[iCurve]), 2) + ((pCoefficient + iCurve)->c1)*(dInX - x[iCurve]) + ((pCoefficient + iCurve)->d0);				
				printf_s("B Output X:%lf Y:%lf \n", dInX, dInY);	

				dInX = (-((pCoefficient + iCurve)->b2) + ((RY1 + RY2) / 2) + ((RY1 - RY2)*pow(3.0,1.0/2.0) / 2)) / 3.0 * ((pCoefficient + iCurve)->a3);
				dInX = x[iCurve] + dInX;
				printf_s("C Output X:%lf Y:%lf \n", dInX, dInY);

				dInY = ((pCoefficient + iCurve)->a3)*pow((dInX - x[iCurve]), 3) + ((pCoefficient + iCurve)->b2)*pow((dInX - x[iCurve]), 2) + ((pCoefficient + iCurve)->c1)*(dInX - x[iCurve]) + ((pCoefficient + iCurve)->d0);
				printf_s("D Output X:%lf Y:%lf \n", dInX, dInY);

				dInX = (-((pCoefficient + iCurve)->b2) + ((RY1 + RY2) / 2) - ((RY1 - RY2)*pow(3.0, 1.0 / 2.0) / 2)) / 3.0 * ((pCoefficient + iCurve)->a3);
				dInX = x[iCurve] + dInX;
				printf_s("C Output X:%lf Y:%lf \n", dInX, dInY);

				dInY = ((pCoefficient + iCurve)->a3)*pow((dInX - x[iCurve]), 3) + ((pCoefficient + iCurve)->b2)*pow((dInX - x[iCurve]), 2) + ((pCoefficient + iCurve)->c1)*(dInX - x[iCurve]) + ((pCoefficient + iCurve)->d0);
				printf_s("D Output X:%lf Y:%lf \n", dInX, dInY);

				//dInX = UnivariateCubiceQuationSolute(((pCoefficient + iCurve)->a3), ((pCoefficient + iCurve)->b2), ((pCoefficient + iCurve)->c1), ((pCoefficient + iCurve)->d0), x[iCurve+1]);
				//dInX = x[iCurve] + dInX;
				//printf_s("C Output X:%lf Y:%lf \n", dInX, dInY);

				//dInY = ((pCoefficient + iCurve)->a3)*pow((dInX - x[iCurve]), 3) + ((pCoefficient + iCurve)->b2)*pow((dInX - x[iCurve]), 2) + ((pCoefficient + iCurve)->c1)*(dInX - x[iCurve]) + ((pCoefficient + iCurve)->d0);
				//printf_s("D Output X:%lf Y:%lf \n", dInX, dInY);
			}
			else if (1e-12 > ABC){
				double r = 0;
				double T = 0;

				T = (2 * A*((pCoefficient + iCurve)->b2) - 3 * ((pCoefficient + iCurve)->a3)*B) / (2 * A*pow(fabs(A), 1.0 / 2.0));	
				
				r = acos(T);

				dInX = (-((pCoefficient + iCurve)->b2) - 2 * pow((double)fabs(A), 1 / 2.0)*cos(r / 3.0)) / (3 * ((pCoefficient + iCurve)->a3));  //
				printf_s("A Output X:%lf Y:%lf \n", dInX, dInY);
				dInX = x[iCurve] + dInX;
				dInY = ((pCoefficient + iCurve)->a3)*pow((dInX - x[iCurve]), 3) + ((pCoefficient + iCurve)->b2)*pow((dInX - x[iCurve]), 2) + ((pCoefficient + iCurve)->c1)*(dInX - x[iCurve]) + ((pCoefficient + iCurve)->d0);
				printf_s("B Output X:%lf Y:%lf \n", dInX, dInY);

				dInX = (-((pCoefficient + iCurve)->b2) + pow(fabs(A), 1.0 / 2.0)*(cos(r / 3.0) + pow(3, 1.0 / 2.0)*sin(r / 3.0))) / (3 * ((pCoefficient + iCurve)->a3));  //
				printf_s("C Output X:%lf Y:%lf \n", dInX, dInY);
				dInX = x[iCurve] + dInX;
				dInY = ((pCoefficient + iCurve)->a3)*pow((dInX - x[iCurve]), 3) + ((pCoefficient + iCurve)->b2)*pow((dInX - x[iCurve]), 2) + ((pCoefficient + iCurve)->c1)*(dInX - x[iCurve]) + ((pCoefficient + iCurve)->d0);
				printf_s("D Output X:%lf Y:%lf \n", dInX, dInY);

				dInX = (-((pCoefficient + iCurve)->b2) + pow(fabs(A), 1.0 / 2.0)*(cos(r / 3.0) - pow(3, 1.0 / 2.0)*sin(r / 3.0))) / (3 * ((pCoefficient + iCurve)->a3));  //
				printf_s("E Output X:%lf Y:%lf \n", dInX, dInY);
				dInX = x[iCurve] + dInX;
				dInY = ((pCoefficient + iCurve)->a3)*pow((dInX - x[iCurve]), 3) + ((pCoefficient + iCurve)->b2)*pow((dInX - x[iCurve]), 2) + ((pCoefficient + iCurve)->c1)*(dInX - x[iCurve]) + ((pCoefficient + iCurve)->d0);
				printf_s("F Output X:%lf Y:%lf \n", dInX, dInY);

			}


		}
		else{
			printf_s("again ");//
		}
	}

	//for (dInX = 0; dInX <= 100; dInX += 0.01){
	//	iCurve = iGetCurve(x, n, dInX);
	//	dInY = ((pCoefficient + iCurve)->a3)*pow((dInX - x[iCurve]), 3) + ((pCoefficient + iCurve)->b2)*pow((dInX - x[iCurve]), 2) + ((pCoefficient + iCurve)->c1)*(dInX - x[iCurve]) + ((pCoefficient + iCurve)->d0);
	//	printf_s("%lf\n", dInY);
	//}

	free(x);
	free(y);
	free(pCoefficient);

	system("pause");
}
#endif

//dInY = ((pCoefficient + iCurve)->a3)*pow((dInX - x[iCurve]), 3) + ((pCoefficient + iCurve)->b2)*pow((dInX - x[iCurve]), 2) + ((pCoefficient + iCurve)->c1)*(dInX - x[iCurve]) + ((pCoefficient + iCurve)->d0);
//A = pow(ABSOLUTE(((pCoefficient + iCurve)->b2)), 2) - 3 * ((pCoefficient + iCurve)->a3)*((pCoefficient + iCurve)->c1);
//B = ((pCoefficient + iCurve)->b2)*((pCoefficient + iCurve)->c1) - 9 * ((pCoefficient + iCurve)->a3)*(((pCoefficient + iCurve)->d0) - dInY);
//C = pow(ABSOLUTE(((pCoefficient + iCurve)->c1)), 2) - 3 * ((pCoefficient + iCurve)->b2)*(((pCoefficient + iCurve)->d0) - dInY);
//ABC = pow(ABSOLUTE(B), 2) - 4 * A*C;
//if (0 == ABC){
//	
//}
//else if (0 < ABC){
//	double Y1, Y2;
//	double RY1, RY2;
//	Y1 = A*((pCoefficient + iCurve)->b2) + 3 * ((pCoefficient + iCurve)->a3)*(((-B + pow(ABSOLUTE(ABC), (double)(1.0 / 2.0))) / 2));
//	Y2 = A*((pCoefficient + iCurve)->b2) + 3 * ((pCoefficient + iCurve)->a3)*(((-B - pow(ABSOLUTE(ABC), (double)(1.0 / 2.0))) / 2));

//	if (Y1 < 0){
//		RY1 = pow(ABSOLUTE(Y1), (double)(1.0 / 3.0));
//		RY1 = -RY1;
//	}
//	else{
//		RY1 = pow(Y1, (double)(1.0 / 3.0));
//	}
//
//	if (Y2 < 0){
//		RY2 = pow(ABSOLUTE(Y2), (double)(1.0 / 3.0));
//		RY2 = -RY2;
//	}
//	else{
//		RY2 = pow(Y2, (double)(1.0 / 3.0));
//	}

//	dInX = (-((pCoefficient + iCurve)->b2) - (RY1 +  RY2)) / 3 * ((pCoefficient + iCurve)->a3);
//	dInX = x[iCurve] + dInX;

//	if (dInX - x[iCurve] < 0){
//		dInY = ((pCoefficient + iCurve)->a3)*(-pow(ABSOLUTE(dInX - x[iCurve]), 3)) + ((pCoefficient + iCurve)->b2)*pow(ABSOLUTE(dInX - x[iCurve]), 2) + ((pCoefficient + iCurve)->c1)*(dInX - x[iCurve]) + ((pCoefficient + iCurve)->d0);
//	}
//	else{
//		dInY = ((pCoefficient + iCurve)->a3)*pow((dInX - x[iCurve]), 3) + ((pCoefficient + iCurve)->b2)*pow((dInX - x[iCurve]), 2) + ((pCoefficient + iCurve)->c1)*(dInX - x[iCurve]) + ((pCoefficient + iCurve)->d0);
//	}
//	
//}
//else if (0 > ABC){

//}


//dInX = dichotomy(((pCoefficient + iCurve)->a3), ((pCoefficient + iCurve)->b2), ((pCoefficient + iCurve)->c1), (((pCoefficient + iCurve)->d0) - dInY), -x[iCurve], x[iCurve + 1]);
//printf_s("C Output X:%lf Y:%lf \n", dInX, dInY);
//dInX = x[iCurve] + dInX;
//dInY = ((pCoefficient + iCurve)->a3)*pow((dInX - x[iCurve]), 3) + ((pCoefficient + iCurve)->b2)*pow((dInX - x[iCurve]), 2) + ((pCoefficient + iCurve)->c1)*(dInX - x[iCurve]) + ((pCoefficient + iCurve)->d0);
//printf_s("D Output X:%lf Y:%lf \n", dInX, dInY);

//dInX = UnivariateCubiceQuation(((pCoefficient + iCurve)->a3), ((pCoefficient + iCurve)->b2), ((pCoefficient + iCurve)->c1), (((pCoefficient + iCurve)->d0) - dInY), x[iCurve], x[iCurve + 1]);
//printf_s("E Output X:%lf Y:%lf \n", dInX, dInY);
//dInX = x[iCurve] + dInX;
//dInY = ((pCoefficient + iCurve)->a3)*pow((dInX - x[iCurve]), 3) + ((pCoefficient + iCurve)->b2)*pow((dInX - x[iCurve]), 2) + ((pCoefficient + iCurve)->c1)*(dInX - x[iCurve]) + ((pCoefficient + iCurve)->d0);
//printf_s("F Output X:%lf Y:%lf \n", dInX, dInY);

//dInY = ((pCoefficient + iCurve)->a3)*pow((dInX - x[iCurve]), 3) + ((pCoefficient + iCurve)->b2)*pow((dInX - x[iCurve]), 2) + ((pCoefficient + iCurve)->c1)*(dInX - x[iCurve]) + ((pCoefficient + iCurve)->d0);