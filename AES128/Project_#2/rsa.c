/*
 * @file    rsa.c
 * @author  작성자 이름 / 학번
 * @date    작성 일자
 * @brief   mini RSA implementation code
 * @details 세부 설명
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "rsa.h"

llint p, q, e, d, n;

llint GCD(llint a, llint b) {
    	llint prev_a;

    	while(b != 0) {
        	printf("GCD(%lld, %lld)\n", a, b);
        	prev_a = a;
        	a = b;
        	while(prev_a >= b) prev_a -= b;
        	b = prev_a;
    	}
    	printf("GCD(%lld, %lld)\n\n", a, b);
    	return a;
}

/*
 * @brief     모듈러 덧셈 연산을 하는 함수.
 * @param     llint a     : 피연산자1.
 * @param     llint b     : 피연산자2.
 * @param     byte op    : +, - 연산자.
 * @param     llint n      : 모듈러 값.
 * @return    llint result : 피연산자의 덧셈에 대한 모듈러 연산 값. (a op b) mod n
 * @todo      모듈러 값과 오버플로우 상황을 고려하여 작성한다.
 */

llint ModAdd(llint a, llint b, byte op, llint n) {
	llint result;
	while(a>=n)a-=n;while(b>=n)b-=n;
	if(op == '+') result = a+b;else result=a-b;
	if(result<0)while(result<0)result +=n;
	while(result>=n)result-=n;
    	return result;
}

/*
 * @brief      모듈러 곱셈 연산을 하는 함수.
 * @param      llint x       : 피연산자1.
 * @param      llint y       : 피연산자2.
 * @param      llint n       : 모듈러 값.
 * @return     llint result  : 피연산자의 곱셈에 대한 모듈러 연산 값. (a x b) mod n
 * @todo       모듈러 값과 오버플로우 상황을 고려하여 작성한다.
 */
llint ModMul(llint x, llint y, llint n) {
	llint result;
	while(x>=n)x-=n;while(y>=n)y-=n;
	result = x * y;
	while(result>=n)result-=n;
	printf("%lld\n", result);
	return result;
}

/*
 * @brief      모듈러 거듭제곱 연산을 하는 함수.
 * @param      llint base   : 피연산자1.
 * @param      llint exp    : 피연산자2.
 * @param      llint n      : 모듈러 값.
 * @return     llint result : 피연산자의 연산에 대한 모듈러 연산 값. (base ^ exp) mod n
 * @todo       모듈러 값과 오버플로우 상황을 고려하여 작성한다.
               'square and multiply' 알고리즘을 사용하여 작성한다.
 */
llint ModPow(llint base, llint exp, llint n) {//solve base^exp mod n = 1, and last solve.
	llint result=base;
	llint time=0;llint t=1;
	while(exp>0)if((t+t)<exp)t+=t;else break;//square
	time=t;result *=result;while(result>=n)result-=n;
	while(exp>time){
		while(exp<time+t)t>>=1;
		time+=t;result *=base;while(result>=n)result-=n;
	}//multiply
	return result;
}

/*
 * @brief      입력된 수가 소수인지 입력된 횟수만큼 반복하여 검증하는 함수.
 * @param      llint testNum   : 임의 생성된 홀수.
 * @param      llint repeat    : 판단함수의 반복횟수.
 * @return     llint result    : 판단 결과에 따른 TRUE, FALSE 값.
 * @todo       Miller-Rabin 소수 판별법과 같은 확률적인 방법을 사용하여,
               이론적으로 4N(99.99%) 이상 되는 값을 선택하도록 한다. 
 */
bool IsPrime(llint testNum, llint repeat) {//1 run/0.25% repeat == 5;a^(d*(2^r))%n =1;
	if(testNum==2||testNum==3)return TRUE;//n<3 handle.
	if(testNum<=1||!(testNum&1))return FALSE;//n<1 and even check.
	int s=0;
	llint d=testNum-1;
	llint temp=0;
	
	while(d>0)if(!(d&1)){d>>=1;s++;}else break;//n-1=d*2^s;
	for(int i=0;i<repeat;i++){
		uint seed = time(NULL);InitWELLRNG512a(&seed);
		llint a=WELLRNG512a()*RND_MAX;while(1)if(a>=testNum-2)a-=(testNum-2);else break;;//1<a<testNum
		a += 2;
		if(temp==a){i--;continue;}
		temp=a;
		if(ModPow(a,d,testNum)==1||ModPow(a,d,testNum)==testNum-1)return TRUE;
		for(int r=0;r<s-1;r++)if(ModPow(a,d*(2^r),testNum)==testNum-1)return TRUE;
	}
	return FALSE;
}

/*
 * @brief       모듈러 역 값을 계산하는 함수.
 * @param       llint a      : 피연산자1.
 * @param       llint m      : 모듈러 값.
 * @return      llint result : 피연산자의 모듈러 역수 값.
 * @todo        확장 유클리드 알고리즘을 사용하여 작성하도록 한다.
 */
llint ModInv(llint a, llint m) {
	llint result=1;//a * result % m == 1;(1+m*t)/a==0;

	for(;ModMul(m,result,a)!=a-1;result++);

	printf("ModInv : %lld\n", result);
	return result;
}

/*
 * @brief     RSA 키를 생성하는 함수.
 * @param     llint *p   : 소수 p.
 * @param     llint *q   : 소수 q.
 * @param     llint *e   : 공개키 값.
 * @param     llint *d   : 개인키 값.
 * @param     llint *n   : 모듈러 n 값.
 * @return    void
 * @todo      과제 안내 문서의 제한사항을 참고하여 작성한다.
 */
void miniRSAKeygen(llint *p, llint *q, llint *e, llint *d, llint *n) {
	llint max, min, lp, temp;

	printf("mRSA key generator start.");
	while(!IsPrime(*p,5)){
		uint seed = time(NULL);InitWELLRNG512a(&seed);
		max = WELLRNG512a()*RND_MAX;min = WELLRNG512a()*RND_MIN;
		*p = min;while(*p > max)*p-=max;
		if(temp==*p)continue;
		printf("random-number1 %lld selected.\n", *p);
		if(IsPrime(*p, 5)){
			printf("%lld may be Prime.\n", *p);
			break;
		}
		else{
			printf("%lld is not Prime.\n", *p);
		}
		temp=*p;
	}
	while(!IsPrime(*q,5)){
		uint seed = time(NULL);InitWELLRNG512a(&seed);
		max = WELLRNG512a()*RND_MAX;min = WELLRNG512a()*RND_MIN;
		*q = min;while(*q > max)*q-=max;
		if(*q==*p&&*q==temp)continue;

		printf("random-number2 %lld selected.\n", *q);
		if(IsPrime(*q, 5)){
			printf("%lld may be Prime.\n", *q);
			break;
		}
		else{
			printf("%lld is not Prime.\n", *q);
		}
		temp=*q;
	}

	*n = (*p) * (*q);
	printf("finally selected prime p, q = %lld, %lld.\nthus, n = %lld\n", *p, *q, *n);
	
	lp = (*p-1) * (*q-1);	

	while(1){
		uint seed = time(NULL);InitWELLRNG512a(&seed);
		max = WELLRNG512a()*RND_MAX;min = WELLRNG512a()*RND_MIN;
		*e = min;while(*e > max)*e-=max;//public key(n,e)
		if(*e<lp && GCD(lp,*e==1))break;
	}
	
	*d = ModInv(*e,lp);
}

/*
 * @brief     RSA 암복호화를 진행하는 함수.
 * @param     llint data   : 키 값.
 * @param     llint key    : 키 값.
 * @param     llint n      : 모듈러 n 값.
 * @return    llint result : 암복호화에 결과값
 * @todo      과제 안내 문서의 제한사항을 참고하여 작성한다.
 */
llint miniRSA(llint data, llint key, llint n) {
	llint result;
	result=data^key;while(result<n)if(result<n)result-=n;
	
	return result;
}

int main(int argc, char* argv[]) {
    byte plain_text[4] = {0x12, 0x34, 0x56, 0x78};
    llint plain_data, encrpyted_data, decrpyted_data;
    uint seed = time(NULL);

    memcpy(&plain_data, plain_text, 4);

    // 난수 생성기 시드값 설정
    seed = time(NULL);
    InitWELLRNG512a(&seed);
    
    // RSA 키 생성
    miniRSAKeygen(&p, &q, &e, &d, &n);
    printf("0. Key generation is Success!\n ");
    printf("p : %lld\n q : %lld\n e : %lld\n d : %lld\n N : %lld\n\n", p, q, e, d, n);

    // RSA 암호화 테스트
    encrpyted_data = miniRSA(plain_data, e, n);
    printf("1. plain text : %lld\n", plain_data);    
    printf("2. encrypted plain text : %lld\n\n", encrpyted_data);

    // RSA 복호화 테스트
    decrpyted_data = miniRSA(encrpyted_data, d, n);
    printf("3. cipher text : %lld\n", encrpyted_data);
    printf("4. Decrypted plain text : %lld\n\n", decrpyted_data);

    // 결과 출력
    printf("RSA Decryption: %s\n", (decrpyted_data == plain_data) ? "SUCCESS!" : "FAILURE!");

    return 0;
}
