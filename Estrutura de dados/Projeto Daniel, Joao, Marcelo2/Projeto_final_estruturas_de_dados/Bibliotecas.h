#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#define min_pix 10 //numero minimo de pixeis aceitaveis por blob

typedef struct
{
	int r, g, b; //cores R, G e B do pixel
	bool verificado; //serve para verificar se o pixel ja está em algum BLOB ou se ja foi analisado
}pixel_info;

typedef struct no_pix
{
	pixel_info *pixel; //acede a informacao do pixel
	int linha, coluna; //guarda a posicao do pixel
	struct no_pix *prox; //aponta para o proximo pixel
}no_pixeis;

typedef struct
{
	no_pixeis *inicio; //aponta para o inicio da lista
	int num_pixeis; //indica o numero de pixeis do blob
}lista_pixeis_blob;

typedef struct no_BLOB
{
	lista_pixeis_blob *pixeis; //aponta para a lista dos pixeis do blob
	struct no_BLOB *prox; //aponta para o proximo blob da lista

	int linha_centro; //guarda a linha do centro do blob
	int coluna_centro; //guarda a coluna do centro do blob
	int distancia; //indica a distancia entre as blobs

	float media_r, media_g, media_b; //guarda a cor media do blob
	float desvio_padrao_r, desvio_padrao_g, desvio_padrao_b; //guarda o desvio padrao das cores dos pixeis do blob
	float desvio_padrao; //guarda o desvio padrao medio das 3 cores
}no_blob;

typedef struct
{
	no_blob *inicio; //aponta para o inicio da lista
	int num_blobs; //indica o numero de blobs da imagem
}lista_blobs;

typedef struct no_imag
{
	lista_blobs *blobs; //aponta para a lista de blobs da imagem
	struct no_imag *prox; //aponta para a proxima imagem da lista
	
	//indica as caracteristicas da imagem
	int linhas; 
	int colunas;
	int canais;
	
	char nome[30]; //indica o nome da imagem
	pixel_info **matriz_pixeis; //guarda ordenadamente os pixeis da imagem
}no_imagens;

typedef struct
{
	no_imagens *inicio, *fim; //aponta para o inicio e fim da lista de imagens
	int num_imagens; //indica o numero de imagens
}lista_imagens;

typedef struct
{
	int mais_blobs; //indica a quantidade de blobs que a imagem tem
	char imag_mais_blobs[30]; //indica o nome da imagem que tem mais blobs

	int maior_distancia; //indica a distancia entre blobs que a imagem tem
	char imag_maior_distancia[30]; //indica o nome da imagem que tem maior distancia

	float menor_desvio; //indica o menor desvio do blob
	no_blob *blob_infos; //aponta para o blob que tem o menor desvio padrao
	char imag_menor_desvio[30]; //indica o nome da imagem que contem o blob com o menor desvio padrao
}max_min;