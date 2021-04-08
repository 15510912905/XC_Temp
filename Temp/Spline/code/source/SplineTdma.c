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
void tdma(float x[], const unsigned int N, const float a[], const float b[], float c[]) 
{
	unsigned int n;
	float m;

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
	for (n = N - 1; n-- > 0; ){                    /* 逆序求出m[i] m[i] = x[i-1]'-c[i]'x[i+1] */ 
		x[n] = x[n] - c[n] * x[n + 1];
	}
	
	/* 此函数返回的解在x[i]中 x[]在此函数中有三层含义 第一次含义为x[i]输入结果值 
	   第二次含义为存储x[i]'的导数 第三次含义为存储得到的m[i]的解  */
}
