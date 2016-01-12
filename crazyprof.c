#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int students = 0;
int inRoom = 0;
int capacity = 0;
int globalID = 0;
int pflag = 0;
pthread_mutex_t roomlock, capslock, idlock, proflock;
pthread_cond_t profcond;

void QuestionDone();
void QuestionStart();
void AnswerStart();
void AnswerDone();
void LeaveOffice();
void EnterOffice();

void * profthread()
{
    while(students > 0)
    {
	 pthread_cond_wait(&profcond, &proflock);
	 AnswerStart(globalID);
         AnswerDone(globalID); 
         pthread_cond_init(&profcond, NULL);
	 if(students == 0)
	     exit(0);
    }
   
}

void *professor()
{
   pthread_t prof;

     pthread_mutex_init(&roomlock, NULL);
     pthread_mutex_init(&capslock, NULL);
     pthread_mutex_init(&idlock, NULL);
     pthread_mutex_init(&proflock, NULL);
     pthread_cond_init(&profcond, NULL);

   if(pthread_create(&prof, NULL, profthread, NULL) != 0)
   {
          perror("Failed to create thread");
          exit(0);
   }

}

void *sthread(void * id)
{
        int able = 0; //student can or can't ask questions. 1 = can
        int questions = numQuestions((int) id);
        while(!able)
        {
                pthread_mutex_lock(&roomlock);
                if(inRoom < capacity){able = 1;}
                else pthread_mutex_unlock(&roomlock);
        }
        while(able){
                inRoom++;
                globalID = (int) id;
                EnterOffice();
             pthread_mutex_unlock(&roomlock);
             break;
        }
        while(questions>0 && able)
        {
                pthread_mutex_lock(&idlock);
                globalID = (int) id;
                QuestionStart(); 
		pthread_cond_signal(&profcond); 
		usleep(1);      
                QuestionDone();
                questions--;
                pthread_mutex_unlock(&idlock);
                usleep(10);
        }
        while(questions == 0 && able)
        {
                pthread_mutex_lock(&capslock);
                globalID = (int) id;
                LeaveOffice();
	        students--;
                inRoom--;
                pthread_mutex_unlock(&capslock);
                break;
        }

}

int numQuestions(int id)
{
   return id % 4 + 1;
}

void *Student(int amount_of_students, int cap)
{
   capacity = cap;
   students = amount_of_students;
   pthread_t student_threads [amount_of_students];
   int i;

   for(i = 0; i < amount_of_students; i++)
   {
      if(pthread_create(&student_threads[i], NULL, sthread, (void *) i) != 0)
      {
          perror("Failed to create thread");
          exit(0);
      }
   }
}

void AnswerStart(int id)
{
    printf("Professor starts to answer the question for student %d\n",
globalID);
}

void AnswerDone(int id)
{
    printf("Professor is done with answer for student %d\n", globalID);
}

void EnterOffice()
{
     printf("Student %d shows up in the office\n" , globalID);
}

void LeaveOffice()
{
     printf("Student %d leaves the office\n", globalID);
}

void QuestionStart()
{
     printf("Student %d asks a question\n", globalID);
}

void QuestionDone()
{
     printf("Student %d is satisfied\n", globalID);
}

int main(int argc, char *argv[])
{
    if((argc != 3) || atoi(argv[1]) < 1 || atoi(argv[2]) < 1)
    {
        printf("Must have 3 arguments and have an integer argument for 2nd and 3rd arguments\n");
        return -1;
    }
    else
    {
        Student(atoi(argv[1]), atoi(argv[2]));
	professor();
        pthread_exit(NULL);
    }
}
