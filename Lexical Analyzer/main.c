#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main(){
	
	FILE *Code, *Lex, *tempFile;
	Code = fopen("code.psi","r");
	Lex = fopen("code.lex","w");
	char character;
	int i, error = 0;
	character = getc(Code);
	while(character != EOF)
	{
		if(character == '\"') //String Constant
		{
			tempFile = fopen("tempFile.txt","w+");
			fprintf(tempFile,"String(");
			character = getc(Code);
			while(character != '\"')
			{
				if(character == EOF)
				{
					printf("[Error] missing terminating \" character");
					error = 1;
					break;
				}
				fprintf(tempFile,"%c",character);
				character = getc(Code);
			}
			fprintf(tempFile,")");
			if(!error)
			{
				rewind(tempFile);
				character = getc(tempFile);
				while(character != EOF)
				{
					fprintf(Lex,"%c",character);
					character = getc(tempFile);
				}
				fprintf(Lex,"\n");
			}
			fclose(tempFile);
			character = getc(Code);
			continue;
		}
		if(character == ';') //End Of Line(;)
		{
			fprintf(Lex,"EndOfLine\n");
			character = getc(Code);
			continue;
		}
		if(character == '(') //LeftPar
		{
			character = getc(Code);
			if(character != '*')
			{
				fprintf(Lex,"LeftPar\n");
				continue;
			}
			else
			{
				character = getc(Code);
				while(character != '*')
				{
					if(character == EOF)
					{
						printf("[Error] Unterminated comment");
						error = 1;
						break;
					}
					character = getc(Code);
				}
				if(!error)
				{
					character = getc(Code);
					if(character == ')')
					{
						character = getc(Code);
						continue;
					}
				}
			}
		}
		if(character == ')') //RightPar
		{
			fprintf(Lex,"RightPar\n");
			character = getc(Code);
			continue;
		}
		if(character == '[') //LeftSquareBracket
		{
			fprintf(Lex,"LeftSquareBracket\n");
			character = getc(Code);
			continue;
		}
		if(character == ']') //RightSquareBracket
		{
			fprintf(Lex,"RightSquareBracket\n");
			character = getc(Code);
			continue;
		}
		if(character == '{') //LeftCurlyBracket
		{
			fprintf(Lex,"LeftCurlyBracket\n");
			character = getc(Code);
			continue;
		}
		if(character == '}') //RightCurlyBracket
		{
			fprintf(Lex,"RightCurlyBracket\n");
			character = getc(Code);
			continue;
		}
		if(character>=48 && character<=57) //Integer Constant
		{
			tempFile = fopen("tempFile.txt","w+");
			fprintf(tempFile,"IntConst(");
			while(character>=48 && character<=57)
			{
				fprintf(tempFile,"%c",character);
				character = getc(Code);
			}
			fprintf(tempFile,")");
			int intSize = ftell(tempFile) - 10; //sizeof(tempFile) - sizeof(IntConst)
			if(intSize > 10)
			{
				printf("[Error] Exceeding length of identifier or numeric constants.");
				error = 1;
			}
			if(!error)
			{
				rewind(tempFile);
				character = getc(tempFile);
				while(character != EOF)
				{
					fprintf(Lex,"%c",character);
					character = getc(tempFile);
				}
				fprintf(Lex,"\n");
			}
			fclose(tempFile);
			fseek(Code,-1,SEEK_CUR);
			character = getc(Code);
			continue;
		}
		if(character == '/' || character == '*') // Operator(/) and Operator(*)
		{
			fprintf(Lex,"Operator(%c)\n",character);
			character = getc(Code);
			continue;
		}
		if(character == '+') //Operator(+) and Operator(++)
		{
			character = getc(Code);
			if(character == '+')
			{
				fprintf(Lex,"Operator(++)\n");
				character = getc(Code);
				continue;
			}
			else
			{
				fprintf(Lex,"Operator(+)\n");
				continue;
			}
		}
		if(character == '-') //Operator(-) and Operator(--)
		{
			character = getc(Code);
			if(character == '-')
			{
				fprintf(Lex,"Operator(--)\n");
				character = getc(Code);
				continue;
			}
			else
			{
				fprintf(Lex,"Operator(-)\n");
				continue;
			}
		}
		if(character == ':') //Operator(:=)
		{
			character = getc(Code);
			if(character == '=')
			{
				fprintf(Lex,"Operator(:=)\n");
				character = getc(Code);
				continue;
			}
			else
				continue;
		}
		if((character>=65 && character<=90) || (character>=97 && character<=122)) //if character is a letter
		{
			tempFile = fopen("tempFile.txt","w+");
			fprintf(tempFile,"%c",character);
			character = getc(Code);
			while((character>=65 && character<=90) || (character>=97 && character<=122) || character == 95 || (character>=48 && character<=57)) //while character is a letter, digit or underscore
			{
				fprintf(tempFile,"%c",character);
				character = getc(Code);
			}
			int wordSize = ftell(tempFile);
			char word[wordSize];
			rewind(tempFile);
			for(i=0; i<wordSize; i++)
				word[i] = tolower(getc(tempFile));
			const char *keywords[] = {"break","case","char","const","continue","do","else","enum","float","for","goto","if","int","long","record","return","static","while"};
			for(i = 0; i<18; i++)
			{
				if(!strcmp(word,keywords[i]))
				{
					fprintf(Lex,"Keyword(%s)\n", word);
					character = getc(Code);
					break;
				}
				else if(i == 17) //if it is not a keyword, it is an identifier
				{
					if(wordSize>20)
					{
						printf("[Error] Exceeding the maximum number of characters.");
						break;
					}
					else
					{
						fprintf(Lex,"Identifier(%s)\n", word);
						break;
					}
				}
			}
			fclose(tempFile);
			fseek(Code,-1,SEEK_CUR);
			character = getc(Code);
			continue;
		}
		character = getc(Code);
	}
	fclose(Code);
	fclose(Lex);
	
	return 0;
}
