#include<unistd.h>
#include<pthread.h>
#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "moduleRadamsa.c"

void input(char*argv[]){
	int error = 0;
	int num=0;

	unsigned char buf[10000];
	unsigned char cmd[20000];

	char* args[3];
	args[0] = argv[0];
	args[1] = argv[2]; // args[1]에 출력디렉토리 대입
	args[2] = 0x0000000000000000;//end args

	while(num++ < 1000){
		struct Ret ret2 = radamsa(0, args);
		if(ret2.len > 10000)
			continue;
		printf("\n\n\n\n");
		write(1, ret2.input, ret2.len);//ret2.input버퍼 포인터에 ret2.len의 바이트 수만큼
		printf("retaddress : %p\n", ret2.input); //ret2.input을 16진수로 출력
		
		memset(buf, 0, 10000); //buf배열을 0으로 초기화
		memcpy(buf, ret.input, ret.len); //ret.len만큼 ret.input을 buf로 복사
		printf("[+] input : %s\n", buf); //buf문자열 출력
		// buf remove \n
		for(int i=0; i<ret.len; i++){
			if(buf[i] == '\x0a'){
				buf[i] = '\x00'; //buf의 줄바꿈문자를 0으로 바꿔줌.
			}
		}
		printf("\n");
		sprintf(cmd, "python -c \'print \"%s\"\' | %s", buf ,argv[1]); //buf주소, argv[1]가 파이썬 실행하도록 cmd에 저장
		printf("[+] cmd : %s\n",cmd);
		int result = system(cmd); //cmd 프로세스 호출하여 실행 후 종료
		printf("[+] Program Terminated\n");
		printf("[+] result: %d/n", result); //cmd 결과값 출력
		if( result != 0)
		{
			printf("error!\n");
			error++;
			// save input file
			char path[100]; 
			sprintf(path, "./%s/%d", argv[3], num); // path 배열에 ./argv[3]num 저장 
			FILE *fp = fopen(path,"a+"); //path파일을 a+모드로 열기
			fwrite(buf, sizeof(buf),1,fp); // buf에 sizeof(buf)바이트를 한번만 파일에 쓰겠다.
			fclose(fp);
		}else{
			printf("good\n");
		}
	}
	printf("[+] Fuzzing Done!\n");
	printf("[+] error : %d\n",error); 
}

int main(int nargs, char*argv[]){
	// Args Check
	// Argv[0] ELF
	if(nargs < 4){
		printf("Usage: ./DumbFuzz [ELFfile] [InputFile] [OutputDir]\n");
		exit(0);
	}
	printf("[+] Start Dumb Fuzzing....\n");

	pthread_t thread_t; //thread_t 정의?
	int status;
	if (pthread_create(&thread_t, NULL, input, argv) < 0) // 스레드 생성 ; 스레드 속성을 기본으로 하고 input에 argv 인자가 전달
	{
		perror("thread create error:");
		exit(0);
	}

	pthread_join(thread_t, (void **)&status); // input함수가 끝날때까지 main에서 기다릴 스레드 tread_t와 리턴값 void**&status
	printf("Thread End %d\n", status); 
	return 1;
}
