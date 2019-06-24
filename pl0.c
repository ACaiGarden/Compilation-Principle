/*
 * PL/0 complier program for win32 platform (implemented in C)
 *
 * The program has been test on Visual C++ 6.0, Visual C++.NET and
 * Visual C++.NET 2003, on Win98, WinNT, Win2000, WinXP and Win2003
 *
 * ʹ�÷�����
 * ���к�����PL/0Դ�����ļ�?
 * �ش��Ƿ���������������
 * �ش��Ƿ��������ֱ�
 * fa.tmp��������������
 * fa1.tmp����Դ�ļ��������ж�Ӧ���׵�ַ
 * fa2.tmp������?
 * fas.tmp�������ֱ�
 */

#include <stdio.h>

#include "pl0.h"
#include "string.h"

#define stacksize 500


int main()
{
	bool nxtlev[symnum];

	printf("Input pl/0 file?   ");
	scanf("%s", fname);     /* �����ļ��� */

	fin = fopen(fname, "r");

	if (fin)
	{
		printf("List object code?(Y/N)");   /* �Ƿ��������������� */
		scanf("%s", fname);
		listswitch = (fname[0]=='y' || fname[0]=='Y');

		printf("List symbol table?(Y/N)");  /* �Ƿ��������ֱ� */
		scanf("%s", fname);
		tableswitch = (fname[0]=='y' || fname[0]=='Y');

		fa1 = fopen("fa1.tmp", "w");
		fprintf(fa1,"Input pl/0 file?   ");
		fprintf(fa1,"%s\n",fname);

		init();     /* ��ʼ�� */

		err = 0;
		cc = cx = ll = 0;
		ch = ' ';

		if(-1 != getsym())
		{
			fa = fopen("fa.tmp", "w");
			fas = fopen("fas.tmp", "w");
			addset(nxtlev, declbegsys, statbegsys, symnum);
			nxtlev[period] = true;

			if(-1 == block(0, 0, nxtlev))   /* ���ñ������� */
			{
				fclose(fa);
				fclose(fa1);
				fclose(fas);
				fclose(fin);
				printf("\n");
				return 0;
			}
			fclose(fa);
			fclose(fa1);
			fclose(fas);

			if (sym != period)
			{
				error(9);
			}

			if (err == 0)
			{
				fa2 = fopen("fa2.tmp", "w");
				interpret();    /* ���ý���ִ�г��� */
				fclose(fa2);
			}
			else
			{
				printf("Errors in pl/0 program");
			}
		}

		fclose(fin);
	}
	else
	{
		printf("Can't open file!\n");
	}

	printf("\n");
	return 0;
}

/*
* ��ʼ��
*/
void init()
{
	int i;

	/* ���õ��ַ����� */
	for (i=0; i<=255; i++)
	{
		ssym[i] = nul;
	}
	ssym['+'] = plus;
	ssym['-'] = minus;
	ssym['*'] = times;
	ssym['/'] = slash;
	ssym['('] = lparen;
	ssym[')'] = rparen;
	ssym['='] = eql;
	ssym[','] = comma;
	ssym['.'] = period;
	ssym['#'] = neq;
	ssym[';'] = semicolon;

	/* ���ñ���������,������ĸ˳�򣬱����۰����� */
	strcpy(&(word[0][0]), "begin");
	strcpy(&(word[1][0]), "call");
	strcpy(&(word[2][0]), "const");
	strcpy(&(word[3][0]), "do");
	strcpy(&(word[4][0]), "end");
	strcpy(&(word[5][0]), "if");
	strcpy(&(word[6][0]), "odd");
	strcpy(&(word[7][0]), "procedure");
	strcpy(&(word[8][0]), "read");
	strcpy(&(word[9][0]), "then");
	strcpy(&(word[10][0]), "var");
	strcpy(&(word[11][0]), "while");
	strcpy(&(word[12][0]), "write");
	strcpy(&(word[13][0]), "for");
	strcpy(&(word[14][0]), "to");
	strcpy(&(word[15][0]), "downto");
	strcpy(&(word[16][0]), "return");

	/* ���ñ����ַ��� */
	wsym[0] = beginsym;
	wsym[1] = callsym;
	wsym[2] = constsym;
	wsym[3] = dosym;
	wsym[4] = endsym;
	wsym[5] = ifsym;
	wsym[6] = oddsym;
	wsym[7] = procsym;
	wsym[8] = readsym;
	wsym[9] = thensym;
	wsym[10] = varsym;
	wsym[11] = whilesym;
	wsym[12] = writesym;
	wsym[13] = forsym;
	wsym[14] = tosym;
	wsym[15] = downtosym;
	wsym[16] = returnsym;

	/* ����ָ������ */
	strcpy(&(mnemonic[lit][0]), "lit");
	strcpy(&(mnemonic[opr][0]), "opr");
	strcpy(&(mnemonic[lod][0]), "lod");
	strcpy(&(mnemonic[sto][0]), "sto");
	strcpy(&(mnemonic[cal][0]), "cal");
	strcpy(&(mnemonic[inte][0]), "int");
	strcpy(&(mnemonic[jmp][0]), "jmp");
	strcpy(&(mnemonic[jpc][0]), "jpc");

	/* ���÷��ż� */
	for (i=0; i<symnum; i++)
	{
		declbegsys[i] = false;
		statbegsys[i] = false;
		facbegsys[i] = false;
	}

	/* ����������ʼ���ż� */
	declbegsys[constsym] = true;
	declbegsys[varsym] = true;
	declbegsys[procsym] = true;

	/* �������俪ʼ���ż� */
	statbegsys[beginsym] = true;
	statbegsys[callsym] = true;
	statbegsys[ifsym] = true;
	statbegsys[whilesym] = true;

	/* �������ӿ�ʼ���ż� */
	facbegsys[ident] = true;
	facbegsys[number] = true;
	facbegsys[lparen] = true;
}

/*
* ������ʵ�ּ��ϵļ�������
*/
int inset(int e, bool* s)
{
	return s[e];
}

int addset(bool* sr, bool* s1, bool* s2, int n)
{
	int i;
	for (i=0; i<n; i++)
	{
		sr[i] = s1[i]||s2[i];
	}
	return 0;
}

int subset(bool* sr, bool* s1, bool* s2, int n)
{
	int i;
	for (i=0; i<n; i++)
	{
		sr[i] = s1[i]&&(!s2[i]);
	}
	return 0;
}

int mulset(bool* sr, bool* s1, bool* s2, int n)
{
	int i;
	for (i=0; i<n; i++)
	{
		sr[i] = s1[i]&&s2[i];
	}
	return 0;
}

/*
*   ������������ӡ����λ�úʹ�������
*/
void error(int n)
{
	char space[81];
	memset(space,32,81);

	space[cc-1]=0; //����ʱ��ǰ�����Ѿ����꣬����cc-1

	printf("****%s!%d\n", space, n);
	fprintf(fa1,"****%s!%d\n", space, n);

	err++;
}


int getch()
{
	if (cc == ll)
	{
		if (feof(fin))
		{
			printf("program incomplete");
			return -1;
		}
		ll=0;
		cc=0;
		printf("%d ", cx);
		fprintf(fa1,"%d ", cx);
		ch = ' ';
		while (ch != 10)
		{
			//fscanf(fin,"%c", &ch)
			//richard
			if (EOF == fscanf(fin,"%c", &ch))
			{
				line[ll] = 0;
				break;
			}
			//end richard
			printf("%c", ch);
			fprintf(fa1, "%c", ch);
			line[ll] = ch;
			ll++;
		}
		printf("\n");
		fprintf(fa1, "\n");
	}
	ch = line[cc];
	cc++;
	return 0;
}


int getsym()
{
	int i,j,k;

	/* the original version lacks "\r", thanks to foolevery */
	while (ch==' ' || ch==10 || ch==9)  /* ���Կո񡢻��С��س���TAB */
	{
		getchdo;
	}
	if (ch>='a' && ch<='z')
	{           /* ���ֻ���������a..z��ͷ */
		k = 0;
		do {
			if(k<al)
			{
				a[k] = ch;
				k++;
			}
			getchdo;
		} while (ch>='a' && ch<='z' || ch>='0' && ch<='9');
		a[k] = 0;
		strcpy(id, a);
		i = 0;
		j = norw-1;
		do {    /* ������ǰ�����Ƿ�Ϊ������ */
			k = (i+j)/2;
			if (strcmp(id,word[k]) <= 0)
			{
				j = k - 1;
			}
			if (strcmp(id,word[k]) >= 0)
			{
				i = k + 1;
			}
		} while (i <= j);
		if (i-1 > j)
		{
			sym = wsym[k];
		}
		else
		{
			sym = ident; /* ����ʧ�����������ֻ����� */
		}
	}
	else
	{
		if (ch>='0' && ch<='9')
		{           /* �����Ƿ�Ϊ���֣���0..9��ͷ */
			k = 0;
			num = 0;
			sym = number;
			do {
				num = 10*num + ch - '0';
				k++;
				getchdo;
			} while (ch>='0' && ch<='9'); /* ��ȡ���ֵ�ֵ */
			k--;
			if (k > nmax)
			{
				error(30);
			}
		}
		else
		{
			if (ch == ':')      /* ���⸳ֵ���� */
			{
				getchdo;
				if (ch == '=')
				{
					sym = becomes;
					getchdo;
				}
				else
				{
					sym = nul;  /* ����ʶ���ķ��� */
				}
			}
			else
			{
				if (ch == '<')      /* ����С�ڻ�С�ڵ��ڷ��� */
				{
					getchdo;
					if (ch == '=')
					{
						sym = leq;
						getchdo;
					}
					else
					{
						sym = lss;
					}
				}
				else
				{
					if (ch=='>')        /* �������ڻ����ڵ��ڷ��� */
					{
						getchdo;
						if (ch == '=')
						{
							sym = geq;
							getchdo;
						}
						else
						{
							sym = gtr;
						}
					}
					else
					{
						sym = ssym[ch];     /* �����Ų�������������ʱ��ȫ�����յ��ַ����Ŵ��� */
						//getchdo;
						//richard
						if (sym != period)
						{
							getchdo;
						}
						//end richard
					}
				}
			}
		}
	}
	return 0;
}

/*
*
*
* x: instruction.f;
* y: instruction.l;
* z: instruction.a;
*/
int gen(enum fct x, int y, int z )
{
	if (cx >= cxmax)
	{
		printf("Program too long"); /* �������� */
		return -1;
	}
	code[cx].f = x;
	code[cx].l = y;
	code[cx].a = z;
	cx++;
	return 0;
}


int test(bool* s1, bool* s2, int n)
{
	if (!inset(sym, s1))
	{
		error(n);

		while ((!inset(sym,s1)) && (!inset(sym,s2)))
		{
			getsymdo;
		}
	}
	return 0;
}

/*
*
*
* lev:    ��ǰ�ֳ������ڲ�
* tx:     ���ֱ���ǰβָ��
* fsys:   ��ǰģ���������ż���
*/
int block(int lev, int tx, bool* fsys)
{
	int i;

	int dx;                 /* ���ַ��䵽�����Ե�ַ */
	int tx0;                /* ������ʼtx */
	int cx0;                /* ������ʼcx */
	bool nxtlev[symnum];    /* ���¼������Ĳ����У����ż��Ͼ�Ϊֵ�Σ�������ʹ������ʵ�֣�
				���ݽ�������ָ�룬Ϊ��ֹ�¼������ı��ϼ������ļ��ϣ������µĿռ�
				���ݸ��¼�����*/

	dx = 3;
	tx0 = tx;               /* ��¼�������ֵĳ�ʼλ�� */
	table[tx].adr = cx;

	gendo(jmp, 0, 0);

	if (lev > levmax)
	{
		error(32);
	}

	do {

		if (sym == constsym)    /* �յ������������ţ���ʼ������������ */
		{
			getsymdo;

			/* the original do...while(sym == ident) is problematic, thanks to calculous */

			do {
				constdeclarationdo(&tx, lev, &dx);  /* dx��ֵ�ᱻconstdeclaration�ı䣬ʹ��ָ�� */
				while (sym == comma)
				{
					getsymdo;
					constdeclarationdo(&tx, lev, &dx);
				}
				if (sym == semicolon)
				{
					getsymdo;
				}
				else
				{
					error(5);   /*©���˶��Ż��߷ֺ�*/
				}
			} while (sym == ident);
		}

		if (sym == varsym)      /* �յ������������ţ���ʼ������������ */
		{
			getsymdo;

			/* the original do...while(sym == ident) is problematic, thanks to calculous */
			do {
				vardeclarationdo(&tx, lev, &dx);
				while (sym == comma)
				{
					getsymdo;
					vardeclarationdo(&tx, lev, &dx);
				}
				if (sym == semicolon)
				{
					getsymdo;
				}
				else
				{
					error(5);
				}
			} while (sym == ident);
		}

		while (sym == procsym) /* �յ������������ţ���ʼ������������ */
		{
			getsymdo;

			if (sym == ident)
			{
				enter(procedur, &tx, lev, &dx); /* ��¼�������� */
				getsymdo;
			}
			else
			{
				error(4);   /* procedure��ӦΪ��ʶ�� */
			}

			if (sym == semicolon)
			{
				getsymdo;
			}
			else
			{
				error(5);   /* ©���˷ֺ� */
			}

			memcpy(nxtlev, fsys, sizeof(bool)*symnum);
			nxtlev[semicolon] = true;
			if (-1 == block(lev+1, tx, nxtlev))
			{
				return -1;  /* �ݹ����� */
			}

			if(sym == semicolon)
			{
				getsymdo;
				memcpy(nxtlev, statbegsys, sizeof(bool)*symnum);
				nxtlev[ident] = true;
				nxtlev[procsym] = true;
				testdo(nxtlev, fsys, 6);
			}
			else
			{
				error(5);   /* ©���˷ֺ� */
			}
		}
		memcpy(nxtlev, statbegsys, sizeof(bool)*symnum);
		nxtlev[ident] = true;
		testdo(nxtlev, declbegsys, 7);
	} while (inset(sym, declbegsys));   /* ֱ��û���������� */

	code[table[tx0].adr].a = cx;    /* ��ʼ���ɵ�ǰ���̴��� */
	table[tx0].adr = cx;            /* ��ǰ���̴�����ַ */
	table[tx0].size = dx;           /* ����������ÿ����һ������������dx����1�����������Ѿ�������dx���ǵ�ǰ�������ݵ�size */
	cx0 = cx;
	gendo(inte, 0, dx);             /* ���ɷ����ڴ����� */

	if (tableswitch)        /* �������ֱ� */
	{
		printf("TABLE:\n");
		if (tx0+1 > tx)
		{
			printf("    NULL\n");
		}
		for (i=tx0+1; i<=tx; i++)
		{
			switch (table[i].kind)
			{
			case constant:
				printf("    %d const %s ", i, table[i].name);
				printf("val=%d\n", table[i].val);
				fprintf(fas, "    %d const %s ", i, table[i].name);
				fprintf(fas, "val=%d\n", table[i].val);
				break;
			case variable:
				printf("    %d var   %s ", i, table[i].name);
				printf("lev=%d addr=%d\n", table[i].level, table[i].adr);
				fprintf(fas, "    %d var   %s ", i, table[i].name);
				fprintf(fas, "lev=%d addr=%d\n", table[i].level, table[i].adr);
				break;
			case procedur:
				printf("    %d proc  %s ", i, table[i].name);
				printf("lev=%d addr=%d size=%d\n", table[i].level, table[i].adr, table[i].size);
				fprintf(fas,"    %d proc  %s ", i, table[i].name);
				fprintf(fas,"lev=%d addr=%d size=%d\n", table[i].level, table[i].adr, table[i].size);
				break;
			}
		}
		printf("\n");
	}

	/* ������������Ϊ�ֺŻ�end */
	memcpy(nxtlev, fsys, sizeof(bool)*symnum);  /* ÿ���������ż��Ͷ������ϲ��������ż��ͣ��Ա㲹�� */
	nxtlev[semicolon] = true;
	nxtlev[endsym] = true;
	statementdo(nxtlev, &tx, lev);
	gendo(opr, 0, 0);                       /* ÿ�����̳��ڶ�Ҫʹ�õ��ͷ����ݶ�ָ�� */
	memset(nxtlev, 0, sizeof(bool)*symnum); /*�ֳ���û�в��ȼ��� */
	testdo(fsys, nxtlev, 8);                /* ��������������ȷ�� */
	listcode(cx0);                          /* �������� */
	return 0;
}

/*
* �����ֱ��м���һ��
*
* k:      ��������const,var or procedure
* ptx:    ���ֱ�βָ����ָ�룬Ϊ�˿��Ըı����ֱ�βָ����ֵ
* lev:    �������ڵĲ���,���Ժ����е�lev��������
* pdx:    dxΪ��ǰӦ�����ı��������Ե�ַ��������Ҫ����1
*/
void enter(enum object k, int* ptx, int lev, int* pdx)
{
	(*ptx)++;
	strcpy(table[(*ptx)].name, id); /* ȫ�ֱ���id���Ѵ��е�ǰ���ֵ����� */
	table[(*ptx)].kind = k;
	switch (k)
	{
	case constant:  /* �������� */
		if (num > amax)
		{
			error(31);  /* ��Խ�� */
			num = 0;
		}
		table[(*ptx)].val = num;
		break;
	case variable:  /* �������� */
		table[(*ptx)].level = lev;
		table[(*ptx)].adr = (*pdx);
		(*pdx)++;
		break;
	case procedur:  /*���������֡�*/
		table[(*ptx)].level = lev;
		break;
	}
}

/*
* �������ֵ�λ��.
* �ҵ��򷵻������ֱ��е�λ��,���򷵻�0.
*
* idt:    Ҫ���ҵ�����
* tx:     ��ǰ���ֱ�βָ��
*/
int position(char* idt, int tx)
{
	int i;
	strcpy(table[0].name, idt);
	i = tx;
	while (strcmp(table[i].name, idt) != 0)
	{
		i--;
	}
	return i;
}


int constdeclaration(int* ptx, int lev, int* pdx)
{
	if (sym == ident)
	{
		getsymdo;
		if (sym==eql || sym==becomes)
		{
			if (sym == becomes)
			{
				error(1);   /* ��=д����:= */
			}
			getsymdo;
			if (sym == number)
			{
				enter(constant, ptx, lev, pdx);
				getsymdo;
			}
			else
			{
				error(2);   /* ����˵��=��Ӧ������ */
			}
		}
		else
		{
			error(3);   /* ����˵����ʶ��Ӧ��= */
		}
	}
	else
	{
		error(4);   /* const��Ӧ�Ǳ�ʶ */
	}
	return 0;
}


int vardeclaration(int* ptx,int lev,int* pdx)
{
	if (sym == ident)
	{
		enter(variable, ptx, lev, pdx); // ��д���ֱ�
		getsymdo;
	}
	else
	{
		error(4);   /* var��Ӧ�Ǳ�ʶ */
	}
	return 0;
}


void listcode(int cx0)
{
	int i;
	if (listswitch)
	{
		for (i=cx0; i<cx; i++)
		{
			printf("%d %s %d %d\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
			fprintf(fa,"%d %s %d %d\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
		}
	}
}


int statement(bool* fsys, int* ptx, int lev)
{
	int i, cx1, cx2;
	bool nxtlev[symnum];

	if (sym == ident)   /* ׼�����ո�ֵ���䴦�� */
	{
		i = position(id, *ptx);
		if (i == 0)
		{
			error(11);  /* ����δ�ҵ� */
		}
		else
		{
			if(table[i].kind != variable)
			{
				error(12);  /* ��ֵ������ʽ���� */
				i = 0;
			}
			else
			{
				getsymdo;
				if(sym == becomes)
				{
					getsymdo;
				}
				else
				{
					error(13);  /* û�м��⵽��ֵ���� */
				}
				memcpy(nxtlev, fsys, sizeof(bool)*symnum);
				expressiondo(nxtlev, ptx, lev); /* ������ֵ�����Ҳ�����ʽ */
				if(i != 0)
				{
					/* expression��ִ��һϵ��ָ������ս������ᱣ����ջ����ִ��sto�������ɸ�ֵ */
					gendo(sto, lev-table[i].level, table[i].adr);
				}
			}
		}//if (i == 0)
	}
	else
	{
		if (sym == readsym) /* ׼������read���䴦�� */
		{
			getsymdo;
			if (sym != lparen)
			{
				error(34);  /* ��ʽ������Ӧ�������� */
			}
			else
			{
				do {
					getsymdo;
					if (sym == ident)
					{
						i = position(id, *ptx); /* ����Ҫ���ı��� */
					}
					else
					{
						i=0;
					}

					if (i == 0)
					{
						error(35);  /* read()��Ӧ���������ı����� */
					}
					else
					{
						gendo(opr, 0, 16);  /* ��������ָ���ȡֵ��ջ�� */
						gendo(sto, lev-table[i].level, table[i].adr);   /* ���浽���� */
					}
					getsymdo;

				} while (sym == comma); /* һ��read�����ɶ��������� */
			}
			if(sym != rparen)
			{
				error(33);  /* ��ʽ������Ӧ�������� */
				while (!inset(sym, fsys))   /* �������ȣ�ֱ���յ��ϲ㺯���ĺ������� */
				{
					getsymdo;
				}
			}
			else
			{
				getsymdo;
			}
		}
		else
		{
			if (sym == writesym)    /* ׼������write���䴦������read���� */
			{
				getsymdo;
				if (sym == lparen)
				{
					do {
						getsymdo;
						memcpy(nxtlev, fsys, sizeof(bool)*symnum);
						nxtlev[rparen] = true;
						nxtlev[comma] = true;       /* write�ĺ�������Ϊ) or , */
						expressiondo(nxtlev, ptx, lev); /* ���ñ���ʽ�������˴���read��ͬ��readΪ��������ֵ */
						gendo(opr, 0, 14);  /* ��������ָ�����ջ����ֵ */
					} while (sym == comma);
					if (sym != rparen)
					{
						error(33);  /* write()��ӦΪ��������ʽ */
					}
					else
					{
						getsymdo;
					}
				}
				gendo(opr, 0, 15);  /* �������� */
			}
			else
			{
				if (sym == callsym) /* ׼������call���䴦�� */
				{
					getsymdo;
					if (sym != ident)
					{
						error(14);  /* call��ӦΪ��ʶ�� */
					}
					else
					{
						i = position(id, *ptx);
						if (i == 0)
						{
							error(11);  /* ����δ�ҵ� */
						}
						else
						{
							if (table[i].kind == procedur)
							{
								gendo(cal, lev-table[i].level, table[i].adr);   /* ����callָ�� */
							}
							else
							{
								error(15);  /* call����ʶ��ӦΪ���� */
							}
						}
						getsymdo;
					}
				}
				else
				{
					if (sym == ifsym)   /* ׼������if���䴦�� */
					{
						getsymdo;
						memcpy(nxtlev, fsys, sizeof(bool)*symnum);
						nxtlev[thensym] = true;
						nxtlev[dosym] = true;   /* ��������Ϊthen��do */
						conditiondo(nxtlev, ptx, lev); /* ���������������߼����㣩���� */
						if (sym == thensym)
						{
							getsymdo;
						}
						else
						{
							error(16);  /* ȱ��then */
						}
						cx1 = cx;   /* ���浱ǰָ����ַ */
						gendo(jpc, 0, 0);   /* ����������תָ���ת��ַδ֪����ʱд0 */
						statementdo(fsys, ptx, lev);    /* ����then�������� */
						code[cx1].a = cx;   /* ��statement��������cxΪthen������ִ������λ�ã�������ǰ��δ������ת��ַ */
					}
					else
					{
						if (sym == beginsym)    /* ׼�����ո������䴦�� */
						{
							getsymdo;
							memcpy(nxtlev, fsys, sizeof(bool)*symnum);
							nxtlev[semicolon] = true;
							nxtlev[endsym] = true;  /* ��������Ϊ�ֺŻ�end */
							/* ѭ���������䴦��������ֱ����һ�����Ų������俪ʼ���Ż��յ�end */
							statementdo(nxtlev, ptx, lev);

							while (inset(sym, statbegsys) || sym==semicolon)
							{
								if (sym == semicolon)
								{
									getsymdo;
								}
								else
								{
									error(10);  /* ȱ�ٷֺ� */
								}
								statementdo(nxtlev, ptx, lev);
							}
							if(sym == endsym)
							{
								getsymdo;
							}
							else
							{
								error(17);  /* ȱ��end���ֺ� */
							}
						}
						else
						{
							if (sym == whilesym)    /* ׼������while���䴦�� */
							{
								cx1 = cx;   /* �����ж�����������λ�� */
								getsymdo;
								memcpy(nxtlev, fsys, sizeof(bool)*symnum);
								nxtlev[dosym] = true;   /* ��������Ϊdo */
								conditiondo(nxtlev, ptx, lev);  /* ������������ */
								cx2 = cx;   /* ����ѭ�����Ľ�������һ��λ�� */
								gendo(jpc, 0, 0);   /* ����������ת��������ѭ���ĵ�ַδ֪ */
								if (sym == dosym)
								{
									getsymdo;
								}
								else
								{
									error(18);  /* ȱ��do */
								}
								statementdo(fsys, ptx, lev);    /* ѭ���� */
								gendo(jmp, 0, cx1); /* ��ͷ�����ж����� */
								code[cx2].a = cx;   /* ��������ѭ���ĵ�ַ����if���� */
							}
							else
							{
								memset(nxtlev, 0, sizeof(bool)*symnum); /* ���������޲��ȼ��� */
								testdo(fsys, nxtlev, 19);   /* ����������������ȷ�� */
							}
						}
					}
				}
			}
		}
	}
	return 0;
}


int expression(bool* fsys, int* ptx, int lev)
{
	enum symbol addop;
	bool nxtlev[symnum];

	if(sym==plus || sym==minus)
	{
		addop = sym;
		getsymdo;
		memcpy(nxtlev, fsys, sizeof(bool)*symnum);
		nxtlev[plus] = true;
		nxtlev[minus] = true;
		termdo(nxtlev, ptx, lev);
		if (addop == minus)
		{
			gendo(opr,0,1);
		}
	}
	else
	{
		memcpy(nxtlev, fsys, sizeof(bool)*symnum);
		nxtlev[plus] = true;
		nxtlev[minus] = true;
		termdo(nxtlev, ptx, lev);
	}
	while (sym==plus || sym==minus)
	{
		addop = sym;
		getsymdo;
		memcpy(nxtlev, fsys, sizeof(bool)*symnum);
		nxtlev[plus] = true;
		nxtlev[minus] = true;
		termdo(nxtlev, ptx, lev);
		if (addop == plus)
		{
			gendo(opr, 0, 2);
		}
		else
		{
			gendo(opr, 0, 3);
		}
	}
	return 0;
}


int term(bool* fsys, int* ptx, int lev)
{
	enum symbol mulop;  /* ���ڱ����˳������� */
	bool nxtlev[symnum];

	memcpy(nxtlev, fsys, sizeof(bool)*symnum);
	nxtlev[times] = true;
	nxtlev[slash] = true;
	factordo(nxtlev, ptx, lev); /* �������� */
	while(sym==times || sym==slash)
	{
		mulop = sym;
		getsymdo;
		factordo(nxtlev, ptx, lev);
		if(mulop == times)
		{
			gendo(opr, 0, 4);   /* ���ɳ˷�ָ�� */
		}
		else
		{
			gendo(opr, 0, 5);   /* ���ɳ���ָ�� */
		}
	}
	return 0;
}


int factor(bool* fsys, int* ptx, int lev)
{
	int i;
	bool nxtlev[symnum];
	testdo(facbegsys, fsys, 24);    /* �������ӵĿ�ʼ���� */
	while(inset(sym, facbegsys))  /* ѭ��ֱ���������ӿ�ʼ���� */
	{
		if(sym == ident)    /* ����Ϊ���������� */
		{
			i = position(id, *ptx); /* �������� */
			if (i == 0)
			{
				error(11);  /* ��ʶ��δ���� */
			}
			else
			{
				switch (table[i].kind)
				{
				case constant:  /* ����Ϊ���� */
					gendo(lit, 0, table[i].val);    /* ֱ�Ӱѳ�����ֵ��ջ */
					break;
				case variable:  /* ����Ϊ���� */
					gendo(lod, lev-table[i].level, table[i].adr);   /* �ҵ�������ַ������ֵ��ջ */
					break;
				case procedur:  /* ����Ϊ���� */
					error(21);  /* ����Ϊ���� */
					break;
				}
			}
			getsymdo;
		}
		else
		{
			if(sym == number)   /* ����Ϊ�� */
			{
				if (num > amax)
				{
					error(31);
					num = 0;
				}
				gendo(lit, 0, num);
				getsymdo;
			}
			else
			{
				if (sym == lparen)  /* ����Ϊ����ʽ */
				{
					getsymdo;
					memcpy(nxtlev, fsys, sizeof(bool)*symnum);
					nxtlev[rparen] = true;
					expressiondo(nxtlev, ptx, lev);
					if (sym == rparen)
					{
						getsymdo;
					}
					else
					{
						error(22);  /* ȱ�������� */
					}
				}
				testdo(fsys, facbegsys, 23);    /* ���Ӻ��зǷ����� */
			}
		}
	}
	return 0;
}


int condition(bool* fsys, int* ptx, int lev)
{
	enum symbol relop;
	bool nxtlev[symnum];

	if(sym == oddsym)   /* ׼������odd���㴦�� */
	{
		getsymdo;
		expressiondo(fsys, ptx, lev);
		gendo(opr, 0, 6);   /* ����oddָ�� */
	}
	else
	{
		/* �߼�����ʽ���� */
		memcpy(nxtlev, fsys, sizeof(bool)*symnum);
		nxtlev[eql] = true;
		nxtlev[neq] = true;
		nxtlev[lss] = true;
		nxtlev[leq] = true;
		nxtlev[gtr] = true;
		nxtlev[geq] = true;
		expressiondo(nxtlev, ptx, lev);
		if (sym!=eql && sym!=neq && sym!=lss && sym!=leq && sym!=gtr && sym!=geq)
		{
			error(20);
		}
		else
		{
			relop = sym;
			getsymdo;
			expressiondo(fsys, ptx, lev);
			switch (relop)
			{
			case eql:
				gendo(opr, 0, 8);
				break;
			case neq:
				gendo(opr, 0, 9);
				break;
			case lss:
				gendo(opr, 0, 10);
				break;
			case geq:
				gendo(opr, 0, 11);
				break;
			case gtr:
				gendo(opr, 0, 12);
				break;
			case leq:
				gendo(opr, 0, 13);
				break;
			}
		}
	}
	return 0;
}


void interpret()
{
	int p, b, t;    /* ָ��ָ�룬ָ����ַ��ջ��ָ�� */
	struct instruction i;   /* ���ŵ�ǰָ�� */
	int s[stacksize];   /* ջ */

	printf("start pl0\n");
	t = 0;
	b = 0;
	p = 0;
	s[0] = s[1] = s[2] = 0;
	do {
		i = code[p];    /* ����ǰָ�� */
		p++;
		switch (i.f)
		{
		case lit:   /* ��a��ֵȡ��ջ�� */
			s[t] = i.a;
			t++;
			break;
		case opr:   /* ��ѧ���߼����� */
			switch (i.a)
			{
			case 0:
				t = b;
				p = s[t+2];
				b = s[t+1];
				break;
			case 1:
				s[t-1] = -s[t-1];
				break;
			case 2:
				t--;
				s[t-1] = s[t-1]+s[t];
				break;
			case 3:
				t--;
				s[t-1] = s[t-1]-s[t];
				break;
			case 4:
				t--;
				s[t-1] = s[t-1]*s[t];
				break;
			case 5:
				t--;
				s[t-1] = s[t-1]/s[t];
				break;
			case 6:
				s[t-1] = s[t-1]%2;
				break;
			case 8:
				t--;
				s[t-1] = (s[t-1] == s[t]);
				break;
			case 9:
				t--;
				s[t-1] = (s[t-1] != s[t]);
				break;
			case 10:
				t--;
				s[t-1] = (s[t-1] < s[t]);
				break;
			case 11:
				t--;
				s[t-1] = (s[t-1] >= s[t]);
				break;
			case 12:
				t--;
				s[t-1] = (s[t-1] > s[t]);
				break;
			case 13:
				t--;
				s[t-1] = (s[t-1] <= s[t]);
				break;
			case 14:
				printf("%d", s[t-1]);
				fprintf(fa2, "%d", s[t-1]);
				t--;
				break;
			case 15:
				printf("\n");
				fprintf(fa2,"\n");
				break;
			case 16:
				printf("?");
				fprintf(fa2, "?");
				scanf("%d", &(s[t]));
				fprintf(fa2, "%d\n", s[t]);
				t++;
				break;
			}
			break;
		case lod:   /* ȡ���Ե�ǰ���̵����ݻ���ַΪa���ڴ���ֵ��ջ�� */
			s[t] = s[base(i.l,s,b)+i.a];
			t++;
			break;
		case sto:   /* ջ����ֵ�浽���Ե�ǰ���̵����ݻ���ַΪa���ڴ� */
			t--;
			s[base(i.l, s, b) + i.a] = s[t];
			break;
		case cal:   /* �����ӹ��� */
			s[t] = base(i.l, s, b); /* �������̻���ַ��ջ */
			s[t+1] = b; /* �������̻���ַ��ջ������������base���� */
			s[t+2] = p; /* ����ǰָ��ָ����ջ */
			b = t;  /* �ı�����ַָ��ֵΪ�¹��̵Ļ���ַ */
			p = i.a;    /* ��ת */
			break;
		case inte:  /* �����ڴ� */
			t += i.a;
			break;
		case jmp:   /* ֱ����ת */
			p = i.a;
			break;
		case jpc:   /* ������ת */
			t--;
			if (s[t] == 0)
			{
				p = i.a;
			}
			break;
		}
	} while (p != 0);
}


int base(int l, int* s, int b)
{
	int b1;
	b1 = b;
	while (l > 0)
	{
		b1 = s[b1];
		l--;
	}
	return b1;
}
