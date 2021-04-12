/*
 * ȭ�ϸ� : my_assembler_00000000.c
 * ��  �� : �� ���α׷��� SIC/XE �ӽ��� ���� ������ Assembler ���α׷��� ���η�ƾ����,
 * �Էµ� ������ �ڵ� ��, ��ɾ �ش��ϴ� OPCODE�� ã�� ����Ѵ�.
 * ���� ������ ���Ǵ� ���ڿ� "00000000"���� �ڽ��� �й��� �����Ѵ�.
 */

 /*
  *
  * ���α׷��� ����� �����Ѵ�.
  *
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>

  // ���ϸ��� "00000000"�� �ڽ��� �й����� ������ ��.
#include "my_assembler_20170819.h"

/* ----------------------------------------------------------------------------------
 * ���� : ����ڷ� ���� ����� ������ �޾Ƽ� ��ɾ��� OPCODE�� ã�� ����Ѵ�.
 * �Ű� : ���� ����, ����� ����
 * ��ȯ : ���� = 0, ���� = < 0
 * ���� : ���� ����� ���α׷��� ����Ʈ ������ �����ϴ� ��ƾ�� ������ �ʾҴ�.
 *		   ���� �߰������� �������� �ʴ´�.
 * ----------------------------------------------------------------------------------
 */
int main(int args, char* arg[]) {

	if (init_my_assembler() < 0) {
		printf("init_my_assembler: ���α׷� �ʱ�ȭ�� ���� �߽��ϴ�.\n");
		return -1;
	}

	if (assem_pass1() < 0) {
		printf("assem_pass1: �н�1 �������� �����Ͽ����ϴ�.  \n");
		return -1;
	}

	make_opcode_output("output_20170819");

	/*
	 * ���� ������Ʈ���� ���Ǵ� �κ�
	 *
	make_symtab_output("symtab_00000000");
	if (assem_pass2() < 0)
	{
		printf(" assem_pass2: �н�2 �������� �����Ͽ����ϴ�.  \n");
		return -1;
	}

	make_objectcode_output("output_00000000");
	*/
	return 0;

}

/* ----------------------------------------------------------------------------------
 * ���� : ���α׷� �ʱ�ȭ�� ���� �ڷᱸ�� ���� �� ������ �д� �Լ��̴�.
 * �Ű� : ����
 * ��ȯ : �������� = 0 , ���� �߻� = -1
 * ���� : ������ ��ɾ� ���̺��� ���ο� �������� �ʰ� ������ �����ϰ� �ϱ�
 *		   ���ؼ� ���� ������ �����Ͽ� ���α׷� �ʱ�ȭ�� ���� ������ �о� �� �� �ֵ���
 *		   �����Ͽ���.
 * ----------------------------------------------------------------------------------
 */
int init_my_assembler(void)
{
	int result;

	if ((result = init_inst_file("inst.data")) < 0)
		return -1;
	if ((result = init_input_file("input.txt")) < 0)
		return -1;
	return result;
}

/* ----------------------------------------------------------------------------------
 * ���� : �ӽ��� ���� ��� �ڵ��� ������ �о� ���� ��� ���̺�(inst_table)��
 *        �����ϴ� �Լ��̴�.
 * �Ű� : ���� ��� ����
 * ��ȯ : �������� = 0 , ���� < 0
 * ���� : ���� ������� ������ �����Ӱ� �����Ѵ�. ���ô� ������ ����.
 *
 *	===============================================================================
 *		   | �̸� | ���� | ���� �ڵ� | ���۷����� ���� | NULL|
 *	===============================================================================
 *
 * ----------------------------------------------------------------------------------
 */
int init_inst_file(char* inst_file)
{
	FILE* file;
	int errno;
	/* add your code here */
	char* token;
	char str[100];
	inst_index = 0;
	
	file = fopen(inst_file, "r");
	if (file < 0) {
		printf("���� �б� ����\n");
	}
	while (!feof(file)) {
		fgets(str, 100, file);
		inst order;
		token = strtok(str, "|");// �������� |�� �������� token�� �ڸ�
		inst_table[inst_index] = (inst*)malloc(sizeof(inst));
		strcpy(order.str, token); // ù��° �ڸ����� str�� �ڵ����� �־���
		strcpy(inst_table[inst_index]->str,order.str); 
		int k = 0;
		while (token != NULL) {
			token = strtok(NULL, "|"); 
			if (k == 0) {
				order.format = atoi(token); //���� ��ȯ�� ���� atoi���
				inst_table[inst_index]->format = order.format;
			}
			else if (k == 1) {
				unsigned int rst = 0;
				rst = strtol(token, NULL, 16); // op�� 16������ �����ϱ� ���� strtol�Լ� ���
				order.op = rst; // �� ����.
				inst_table[inst_index]->op = order.op;
			}
			else if (k == 2) {
				order.ops = atoi(token); // ���� ��ȯ�� ���� atoi���
				inst_table[inst_index]->ops = order.ops;
			}
			k++; // ���� token�� �������� ����.
		}
		inst_index++; // �ϳ��� table�� ä��� ���� table�� ä��� ���� ����.
	}
	fclose(file);
	return errno;
}

/* ----------------------------------------------------------------------------------
 * ���� : ����� �� �ҽ��ڵ带 �о� �ҽ��ڵ� ���̺�(input_data)�� �����ϴ� �Լ��̴�.
 * �Ű� : ������� �ҽ����ϸ�
 * ��ȯ : �������� = 0 , ���� < 0
 * ���� : ���δ����� �����Ѵ�.
 *
 * ----------------------------------------------------------------------------------
 */
int init_input_file(char* input_file)
{
	FILE* file;
	int errno;
	/* add your code here */
	line_num = 0;
	file = fopen(input_file, "r");
	if (file < 0) {
		printf("file open error\n");
	}
	while (!feof(file)) { // file�� ó������ ������ ����
		input_data[line_num] = (char*)malloc(sizeof(char)*MAX_LINES);
		fgets(input_data[line_num], MAX_LINES, file); //fgets�� ������ ���������� ����. �� line���� �д´�.
		line_num++; //���� �а� ����
	}
	fclose(file);
	return errno;
}

/* ----------------------------------------------------------------------------------
 * ���� : �ҽ� �ڵ带 �о�� ��ū������ �м��ϰ� ��ū ���̺��� �ۼ��ϴ� �Լ��̴�.
 *        �н� 1�� ���� ȣ��ȴ�.
 * �Ű� : �Ľ��� ���ϴ� ���ڿ�
 * ��ȯ : �������� = 0 , ���� < 0
 * ���� : my_assembler ���α׷������� ���δ����� ��ū �� ������Ʈ ������ �ϰ� �ִ�.
 * ----------------------------------------------------------------------------------
 */
int token_parsing(char* str) {
	/* add your code here */
	char* piece;
	int i = 0;
	int j = 0;
	token unit; // token����ü
	token_table[token_line] = (token*)malloc(sizeof(token)); // tokentable����
	memset(&unit, 0, sizeof(struct token_unit)); // �޸� �ʱ�ȭ

	for (int i = 0; i < MAX_OPERAND; i++) {
		memset(token_table[token_line]->operand[i], 0, sizeof(token_table[token_line]->operand[i]));
	} // operand�� ������ ���� �ʱ�ȭ
	
	piece = strtok(str, "\t");// tab�� �������� token���� �ڸ�
	while (piece != NULL) {
		if (i == 0) {
			unit.label = piece;
			token_table[token_line]->label = unit.label; // ù��° token�� label
		//	printf("label %s \n",token_table[token_line]->label);
		}
		if (i == 1) {
			unit.operator = piece;
			token_table[token_line]->operator = unit.operator; // �ι�° token�� operator
		//	printf("operator %s \n", token_table[token_line]->operator);
		}
		else if (i == 2) { // ����° token�� operand
			int num = 0;
			int flag = 0; // flag������ ���� opreand�� ������ �ľ�
			for (int i = 0; i < strlen(piece); i++) {
				char temp[20] = {NULL, };
				if (piece[i] == ',' && flag == 0) {  // ,�� �ϳ��ִ�. �� operand�� 2�� ù��° operand�� �����
					for (int j = 0; j < i; j++) {
						temp[j] = piece[j];
					}
					strcpy(unit.operand[j],temp);
					strcpy(token_table[token_line]->operand[j], unit.operand[j]);
				//	printf("operand %d %s\n", j, token_table[token_line]->operand[j]);
					j++;
					flag++;
					num = i + 1;
				} 
				else if (piece[i] == ',' && flag == 1) { // ,�� �ϳ� �� �߰�. �� operand�� 3��
					int second = 0;
					for (int j = num; j < i; j++) {
						temp[second] = piece[j];
						second++;
						num = i + 1;
					}
					strcpy(unit.operand[j], temp);
					strcpy(token_table[token_line]->operand[j], unit.operand[j]); // �ٷ� 2��° operand�� �־��ֱ�
				//	printf("operand %d %s\n", j, token_table[token_line]->operand[j]);
					j++;
					int thrd = 0;
					char temp2[20] = { NULL, };
					for (int k = num; k < strlen(piece); k++) {
						temp2[thrd] = piece[k];
						thrd++;
					}
					strcpy(unit.operand[j], temp2);
					strcpy(token_table[token_line]->operand[j], unit.operand[j]);// 3��° operand�� �־��ֱ�
				//	printf("operand %d %s\n", j, token_table[token_line]->operand[j]);
					j++;
					break;
				}
				else if (i == strlen(piece) - 1 && flag == 1) { // ,�� �ϳ� �ι�° operand�� ����ش�.
					int second = 0;
					for (int j = num; j < strlen(piece); j++) {
						temp[second] = piece[j];
						second++;
						num = i + 1;
					}
					strcpy(unit.operand[j], temp);
					strcpy(token_table[token_line]->operand[j], unit.operand[j]);
				//	printf("operand %d %s\n", j, token_table[token_line]->operand[j]);
					j++;
				}
				else if (i == strlen(piece) - 1 && flag == 0) { // operand�� �Ѱ�
					strcpy(unit.operand[j], piece);
					strcpy(token_table[token_line]->operand[j], unit.operand[j]);
				//	printf("operand %d %s\n", j,token_table[token_line]->operand[j]);
					j++;
				}
			}
		}

		else if (i == 3) {
			strcpy(unit.comment,piece);
			strcpy(token_table[token_line]->comment, unit.comment);
			//printf("coment %s\n", token_table[token_line]->comment);
		}
		piece = strtok(NULL, "\t");
		i++;
	}
	token_line ++;
}


/* ----------------------------------------------------------------------------------
 * ���� : �Է� ���ڿ��� ���� �ڵ������� �˻��ϴ� �Լ��̴�.
 * �Ű� : ��ū ������ ���е� ���ڿ�
 * ��ȯ : �������� = ���� ���̺� �ε���, ���� < 0
 * ���� :
 *
 * ----------------------------------------------------------------------------------
 */
int search_opcode(char* str)
{
	/* add your code here */
		for (int i = 0; i < inst_index; i++) {
			if (strcmp(str, inst_table[i]->str) == 0) { // ã���� �� �ش� index ��ȯ
				return i;
				break;
			}
		}
	return -1;
}

/* ----------------------------------------------------------------------------------
* ���� : ����� �ڵ带 ���� �н�1������ �����ϴ� �Լ��̴�.
*		   �н�1������..
*		   1. ���α׷� �ҽ��� ��ĵ�Ͽ� �ش��ϴ� ��ū������ �и��Ͽ� ���α׷� ���κ� ��ū
*		   ���̺��� �����Ѵ�.
*
* �Ű� : ����
* ��ȯ : ���� ���� = 0 , ���� = < 0
* ���� : ���� �ʱ� ���������� ������ ���� �˻縦 ���� �ʰ� �Ѿ �����̴�.
*	  ���� ������ ���� �˻� ��ƾ�� �߰��ؾ� �Ѵ�.
*
* -----------------------------------------------------------------------------------
*/
static int assem_pass1(void)
{
	/* add your code here */
	
	for (int i = 0; i < line_num; i++) {
		token_parsing(input_data[i]); // �ѹ��徿 �־��ֱ�
	}
	/* input_data�� ���ڿ��� ���پ� �Է� �޾Ƽ�
	 * token_parsing()�� ȣ���Ͽ� token_unit�� ����
	 */
}

/* ----------------------------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ ��ɾ� ���� OPCODE�� ��ϵ� ǥ(���� 3��) �̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*        ���� ���� 3�������� ���̴� �Լ��̹Ƿ� ������ ������Ʈ������ ������ �ʴ´�.
* -----------------------------------------------------------------------------------
*/

void make_opcode_output(char* file_name)
{
	/* add your code here */
	
	char buffer[MAX_LINES]; // outputfile�� �ۼ��� buffer
	
	if (file_name == NULL) {
		for (int i = 0; i < line_num; i++) {
			int j = 0;
			int opnum = 0;
			strcpy(buffer, token_table[i]->label);
			strcat(buffer, "\t");
			//printf("%s\t",token_table[i]->label);

			strcat(buffer, token_table[i]->operator);
			strcat(buffer, "\t");

			//printf("%s\t", token_table[i]->operator);

			//printf("%s",token_table[i]->operand[j]);
			strcat(buffer, token_table[i]->operand[j]);


			if (token_table[i]->operand[j + 1][0] != NULL) {
				//	printf(",%s", token_table[i]->operand[j+1]);
				strcat(buffer, ",");
				strcat(buffer, token_table[i]->operand[j + 1]);
			}

			if (token_table[i]->operand[j + 2][0] != NULL) {
				//printf(",%s", token_table[i]->operand[j + 2]);
				strcat(buffer, ",");
				strcat(buffer, token_table[i]->operand[j + 1]);
			}
			strcat(buffer, "\t");
			//printf("\t");
			if (strchr(token_table[i]->operator,'+') != NULL) {
				token_table[i]->operator++;
			}
			opnum = search_opcode(token_table[i]->operator);

			fprintf(stdout, "%s", buffer);

			if (opnum != -1) {
				unsigned char a, b;
				a = inst_table[opnum]->op;
				b = inst_table[opnum]->op;
				printf("%X", a >> 4);
				printf("%X\t", b & 0x0F);
			}
			if (i == line_num - 1) {
				break;
			}
			
			printf("\n");
		}
	}

	else {
	FILE* fp;
	fp = fopen(file_name, "w");
	for (int i = 0; i < line_num; i++) {
		int j = 0;
		int opnum = 0;
		strcpy(buffer, token_table[i]->label);
		strcat(buffer, "\t");
		//printf("%s\t",token_table[i]->label);

		strcat(buffer, token_table[i]->operator);
		strcat(buffer, "\t");

		//printf("%s\t", token_table[i]->operator);

		//printf("%s",token_table[i]->operand[j]);
		strcat(buffer, token_table[i]->operand[j]);


		if (token_table[i]->operand[j + 1][0] != NULL) {
			//	printf(",%s", token_table[i]->operand[j+1]);
			strcat(buffer, ",");
			strcat(buffer, token_table[i]->operand[j + 1]);
		}

		if (token_table[i]->operand[j + 2][0] != NULL) {
			//printf(",%s", token_table[i]->operand[j + 2]);
			strcat(buffer, ",");
			strcat(buffer, token_table[i]->operand[j + 1]);
		}
		strcat(buffer, "\t");
		//printf("\t");
		if (strchr(token_table[i]->operator,'+') != NULL) {
			token_table[i]->operator++;
		}
		opnum = search_opcode(token_table[i]->operator);

		fprintf(fp, "%s", buffer);

		if (opnum != -1) {
			unsigned char a, b;
			a = inst_table[opnum]->op;
			b = inst_table[opnum]->op;
			fprintf(fp, "\t%X", a >> 4);
			fprintf(fp, "%X", b & 0x0F);
			//printf("%X", a >> 4);
			//printf("%X\t", b & 0x0F);
		}
		if (i == line_num - 1) {
			break;
		}
		fprintf(fp, "%s", "\n");
		//	printf("\n");
	}
	
}

}

/* ----------------------------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ SYMBOL�� �ּҰ��� ����� TABLE�̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*
* -----------------------------------------------------------------------------------
*/
void make_symtab_output(char* file_name)
{
	/* add your code here */
}

/* ----------------------------------------------------------------------------------
* ���� : ����� �ڵ带 ���� �ڵ�� �ٲٱ� ���� �н�2 ������ �����ϴ� �Լ��̴�.
*		   �н� 2������ ���α׷��� ����� �ٲٴ� �۾��� ���� ������ ����ȴ�.
*		   ������ ���� �۾��� ����Ǿ� ����.
*		   1. ������ �ش� ����� ��ɾ ����� �ٲٴ� �۾��� �����Ѵ�.
* �Ű� : ����
* ��ȯ : �������� = 0, �����߻� = < 0
* ���� :
* -----------------------------------------------------------------------------------
*/
static int assem_pass2(void)
{

	/* add your code here */
}

/* ----------------------------------------------------------------------------------
* ���� : �Էµ� ���ڿ��� �̸��� ���� ���Ͽ� ���α׷��� ����� �����ϴ� �Լ��̴�.
*        ���⼭ ��µǴ� ������ object code (������Ʈ 1��) �̴�.
* �Ű� : ������ ������Ʈ ���ϸ�
* ��ȯ : ����
* ���� : ���� ���ڷ� NULL���� ���´ٸ� ���α׷��� ����� ǥ��������� ������
*        ȭ�鿡 ������ش�.
*
* -----------------------------------------------------------------------------------
*/
void make_objectcode_output(char* file_name)
{
	/* add your code here */
}
