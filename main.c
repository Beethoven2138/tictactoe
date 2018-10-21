//I DID NOT WRITE THE UI, ONLY THE AI


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
 
/*print matrix*/
void printMatrix(char val);
/*update value in matrix*/
void insertValue(int i,int j,unsigned char user);
/*check matrix is full or not*/
unsigned char isFull(void);
/*check game is completed or not.*/
unsigned char isOver(char *matrix);
 
unsigned char mat[9]={'_','_','_','_','_','_','_','_','_'};
unsigned char usertern=0;
unsigned char onlyOnce=0;

typedef struct _node
{
	char player; //1 == player1, 0 == player2
	int value;
	unsigned char matrix[9];
	int child_cnt;
	struct _node *child;
} NODE;

//int children_count[8] = {9, 9*8, 9*8*7, 9*8*7*6, 9*8*7*6*5, 9*8*7*6*5*4, 9*8*7*6*5*4*3, 9*8*7*6*5*4*3*2};

NODE *start;
NODE *current;
char glb_player;

void init_tree(char player);
void build_children(NODE *parent, int level/*of children*/)
{
	if (level == 9)
	{
		parent->child = NULL;
		parent->child_cnt = 0;
		parent->value = 0;
		return;
	}
	int tmp_ccnt = 0;
	parent->child = (NODE*)malloc(sizeof(NODE) * (9-level));
	parent->child_cnt = 9-level;
	for (int y = 0; y < 3; ++y)
	{
		for (int x = 0; x < 3; ++x)
		{
			if (parent->matrix[y*3+x] == '_')
			{
				memcpy(parent->child[tmp_ccnt].matrix, parent->matrix, 9);
				parent->child[tmp_ccnt].matrix[y*3+x] = (parent->player == 0) ? 'X' : 'O';
				parent->child[tmp_ccnt].player = !parent->player;
				if (isOver(parent->child[tmp_ccnt].matrix))
				{
					parent->child[tmp_ccnt].child = NULL;
					parent->child[tmp_ccnt].child_cnt = 0;
					if (parent->child[tmp_ccnt].player == glb_player)
						parent->child[tmp_ccnt].value = 1;
					else
						parent->child[tmp_ccnt].value = -1;
					++tmp_ccnt;
				}
				else
					build_children(&parent->child[tmp_ccnt++], level+1);
			}
		}
	}
	parent->value = (parent->player == glb_player) ? 5 : -5;
	for (int i = 0; i < parent->child_cnt; ++i)
	{
		if (parent->player == !glb_player)
		{
			if (parent->child[i].value > parent->value)
			{
				parent->value = parent->child[i].value;
				if (parent->value == 1)
					return;
			}
		}
		else if (parent->player == glb_player)
		{
			if (parent->child[i].value < parent->value)
			{
				parent->value = parent->child[i].value;
				if (parent->value == -1)
					return;
			}
		}
	}
}

void destroy_tree(NODE *node)
{
	if (node && node->child == NULL)
	{
		free(node);
		return;
	}
	for (int i = 0; i < node->child_cnt; ++i)
	{
		if (&node->child[i])
			destroy_tree(&node->child[i]);
	}
}

void init_tree(char player)
{
	glb_player = player;
	start = (NODE*)malloc(sizeof(NODE));
	start->player = !player;
	current = start;
	if (player == 1)
	{
		for (int y = 0; y < 3; ++y)
		{
			for (int x = 0; x < 3; ++x)
				start->matrix[y*3+x] = mat[y*3+x];
		}

		build_children(start, 0);
	}
	
}

void ai_move(void)
{
	int node = -5;
	for (int i = 0; i < current->child_cnt; ++i)
	{
		if (current->child[i].value == 1 && current->child[i].child_cnt == 0)
		{
			node = i;
			break;
		}
		if (current->child[i].value >= 0)
		{
			node = i;
		}
	}
	if (node == -5)
		node = 0;
	memcpy(mat, current->child[node].matrix, 9);
	current = &current->child[node];
}

void human_move(void)
{
	for (int i = 0; i < current->child_cnt; ++i)
	{
		if (memcmp(current->child[i].matrix, mat, 9) == 0)
		{
			current = &current->child[i];
			return;
		}
	}
	assert(0);
}

int main(int argc, char **argv)
{	
	unsigned char user1[30],user2[30],winner;
	int ival,jval;
 
	printf("\nEnter name of user1 :"); 
	//gets(user1);
	scanf("%s", user1);
	fflush(stdin);

	printf("Enter name of user2 :"); 
	//gets(user2);
	scanf("%s", user2);
	fflush(stdin);

	init_tree(1);
	//destroy_tree(start);

again:
	system("clear");
	printf("\n\n\n\n");
	if(!onlyOnce) {printMatrix(0); onlyOnce=1;}
	else { printMatrix(1);}
	winner=isOver(mat);

	if(winner)
	{
		printf("\n *** Congratulations Dear %s ,\n     You have won the game . !!!!",((winner=='X')?user1:user2));
		goto end;
	}
 
	if(!isFull()) {printf("\n *** Game Over ."); goto end;}

repeat:
	if (glb_player == usertern)
	{
		fflush(stdin);
		printf("\n *** %s , Enter value (00-22) seperated by space :",(usertern==0x00?user1:user2));
		scanf("%d%d",&ival,&jval);
		if( (ival>2 || ival<0) || (jval>2 || jval<0))
		{
			printf("\n *** ERROR : Invalid index, try again !!!"); 
			goto repeat;
		}
		if(mat[ival*3+jval]!='_'){printf("\n *** ERROR : Already filled,try again !!!"); goto repeat;}
		insertValue(ival,jval,usertern);
		human_move();
	}
	else
	{
		ai_move();
	}
	usertern=!usertern;
	goto again;

end:
	printf("\n");return 0;
}

void printMatrix(char val)
{
	unsigned char i,j;
	if(!val){
		/*For blank matrix*/
 
		for(i=0;i<3;i++){    
			printf("\t\t\t");       
			for(j=0;j<3;j++){printf("[%3c ] ",mat[i*3+j]);} printf("\n");
		}
 
		return;
	}
	for(i=0;i<3;i++){
		printf("\t\t\t");
		for(j=0;j<3;j++){printf("[%3c ] ",mat[i*3+j]);} printf("\n");
	}
	return;
}

void insertValue(int i,int j,unsigned char user){
//  printf("\n ##### %d, %d ###\n",i,j);
	mat[i*3+j]=((user==0x00)?'X':'O');
}

unsigned char isFull(void){
	unsigned char i,j,count=0;;
	for(i=0;i<3;i++)
		for(j=0;j<3;j++){ if(mat[i*3+j]=='_') ++count; }
	return count;
}

unsigned char isOver(char *matrix){
     
	unsigned char i,j,user;
	unsigned char storeChar=0,flag;
	int sum=0;
	// case 1
	/*sum of 'X'+'X'+'X' = 264 and sum of 'O'+'O'+'O' =237*/
	flag=0;
	for(i=0;i<3;i++)
	{   sum=0;
		for(j=0;j<3;j++)
		{ 
			if(matrix[i*3+j]=='_') break;
			sum+=matrix[i*3+j];
			storeChar=matrix[i*3+j];
		}
         
		if(sum==237||sum==264){return storeChar;}
	}

	// case 2
	for(i=0;i<3;i++)
	{   sum=0;
		for(j=0;j<3;j++)
		{ 
			if(matrix[j*3+i]=='_') break;
			sum+=matrix[j*3+i];
			storeChar=matrix[j*3+i];
		}
		if(sum==237||sum==264){return storeChar;}
	}

	// case 3

	for(i=0;i<3;i++)
	{   
		sum=0;
		for(j=0;j<3;j++)
		{ 
			if(i==j){           
				if(matrix[i*3+j]=='_') break;
				sum+=matrix[i*3+j];
				storeChar=matrix[i*3+j];
			}
		}
		if(sum==237||sum==264){ return storeChar;}
	}

	sum=0;
	for (i = 0; i < 3; ++i)
	{
		if(matrix[i*3+i]=='_') break;
		sum+=matrix[i*3+i];
		storeChar=matrix[i*3+i];
		if(sum==237||sum==264){ return storeChar;}
	}

	sum=0;
	for (i = 0; i < 3; ++i)
	{
		if(matrix[i*3+2-i]=='_') break;
		sum+=matrix[i*3+2-i];
		storeChar=matrix[i*3+2-i];
		if(sum==237||sum==264){ return storeChar;}
	}

	return 0;

}
