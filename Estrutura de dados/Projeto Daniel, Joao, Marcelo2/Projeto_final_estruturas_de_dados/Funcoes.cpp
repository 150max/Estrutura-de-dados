#include "Bibliotecas.h"

int validar_argumentos(char **argumentos, int num_argumentos)
{
	FILE *f;
	int aux;
	if (num_argumentos != 8) //numero de argumentos necessario excedido ou nao alcancado
		return -1;

	if (strcmp(argumentos[1], "Testes") != 0) //nao foi escrito Testes na posicao 1
		return 1;
	f = fopen(argumentos[2], "r"); //tentativa de abrir o ficheiro
	if (!f) //o ficheiro nao existe ou nao e possivel abrir
		return 2;
	else //o ficheiro existe
		fclose(f);
	aux = atoi(argumentos[3]); //cor vermelha invalida
	if (aux < 0 || aux > 255)
		return 3;
	aux = atoi(argumentos[4]); //cor verde invalida
	if (aux < 0 || aux > 255)
		return 4;
	aux = atoi(argumentos[5]); //cor azul invalida
	if (aux < 0 || aux > 255)
		return 5;
	aux = atoi(argumentos[6]); //tolerancia invalida
	if (aux < 0 || aux > 255)
		return 6;

	if (strcmp(argumentos[7], "MEM") == 0 || strcmp(argumentos[7], "ALL") == 0 || strcmp(argumentos[7], "MENUS") == 0) //escolhida uma opcao corretamente
		return 0;
	else
		return 7;
}

pixel_info **criar_matriz_pixeis(int linhas, int colunas)
{
	pixel_info **aux = (pixel_info**)malloc(linhas * sizeof(pixel_info*)); //cria as linhas da matriz

	for (int i = 0; i < linhas; i++)
	{
		aux[i] = (pixel_info*)malloc(colunas * sizeof(pixel_info)); //cria as colunas da matriz
	}
	return aux;
}

no_imagens *criar_imagem()
{
	no_imagens *aux = (no_imagens*)malloc(sizeof(no_imagens));

	aux->prox = NULL;
	aux->blobs = NULL;

	return aux;
}

lista_imagens *criar_lista_imagens()
{
	lista_imagens *aux = (lista_imagens*)malloc(sizeof(lista_imagens));

	aux->num_imagens = 0;
	aux->fim = NULL;
	aux->inicio = NULL;

	return aux;
}

lista_pixeis_blob *criar_lista_pixeis_blob()
{
	lista_pixeis_blob *aux = (lista_pixeis_blob*)malloc(sizeof(lista_pixeis_blob));

	aux->inicio = NULL;
	aux->num_pixeis = 0;
	return aux;
}

no_blob *criar_blob()
{
	no_blob *aux = (no_blob*)malloc(sizeof(no_blob));

	aux->prox = NULL;
	aux->pixeis = criar_lista_pixeis_blob();

	aux->coluna_centro = 0;
	aux->linha_centro = 0;
	aux->media_r = 0;
	aux->media_g = 0;
	aux->media_b = 0;

	return aux;
}

lista_blobs *criar_lista_blob()
{
	lista_blobs *aux = (lista_blobs*)malloc(sizeof(lista_blobs));

	aux->inicio = NULL;
	aux->num_blobs = 0;

	return aux;
}

no_pixeis *criar_no_pixel(pixel_info *pixel, int linha, int coluna)
{
	no_pixeis *aux = (no_pixeis*)malloc(sizeof(no_pixeis));

	aux->coluna = coluna;
	aux->linha = linha;
	aux->pixel = pixel;
	aux->prox = NULL;

	return aux;
}

max_min *criar_max_min()
{
	max_min *aux = (max_min*)malloc(sizeof(max_min));

	aux->maior_distancia = -1;
	aux->mais_blobs = -1;
	aux->menor_desvio = 300;
	aux->blob_infos = NULL;

	return aux;
}

void destruir_matriz_pixeis(pixel_info **matriz, int linhas)
{
	for (int i = 0; i < linhas; i++) //eliminar colunas
	{
		free(matriz[i]);
	}
	free(matriz); //eliminar linhas
}

void destruir_lista_pixeis_blob(lista_pixeis_blob *pixeis)
{
	no_pixeis *aux = pixeis->inicio, *libertar;
	while (aux != NULL)
	{
		libertar = aux; //definir o pixel a eliminar
		aux = aux->prox; //atualizar inicio da lista
		free(libertar); //eliminar pixel
	}
	free(pixeis); //eliminar lista
}

void destruir_lista_blobs(lista_blobs *blobs)
{
	if (blobs == NULL)
		return;
	no_blob *aux = blobs->inicio, *libertar;
	
	while (aux != NULL)
	{
		libertar = aux;
		aux = aux->prox;
		destruir_lista_pixeis_blob(libertar->pixeis);
		free(libertar);
	}
	free(blobs);
}

void destruir_imagem(no_imagens *imagem)
{
	if (imagem == NULL)
		return;
	destruir_matriz_pixeis(imagem->matriz_pixeis, imagem->linhas); //destruir matriz dos pixeis
	destruir_lista_blobs(imagem->blobs); //destruir lista de blobs
	free(imagem); //eliminar imagem
}

void destruir_lista_imagens(lista_imagens **imagens)
{
	if ((*imagens) == NULL)
		return;

	no_imagens *aux = (*imagens)->inicio, *libertar;
	
	while (aux != NULL)
	{
		libertar = aux;
		aux = aux->prox;
		destruir_imagem(libertar);
	}
	free((*imagens));
	(*imagens) = NULL;
}

void adicionar_imagem_lista(no_imagens *adicionar, lista_imagens *lista)
{
	if (lista->inicio == NULL) //se a lista estiver vazia
	{
		//adicionar e o primeiro e o ultimo da lista
		lista->inicio = adicionar;
		lista->fim = adicionar;
	}
	else
	{
		//adicionar e o ultimo e e o que esta a seguir ao anterior ultimo
		lista->fim->prox = adicionar;
		lista->fim = adicionar;
	}
	lista->num_imagens++; //incrementa o numero de imagens da lista 
}

bool carregar_do_ficheiro(char *nome_ficheiro, lista_imagens **imagens)
{
	if ((*imagens) != NULL)
	{
		printf("Os dados ja tinham sido carregados antes!\n\n");
		return true;
	}

	FILE *f = fopen(nome_ficheiro, "r"); //abrir ficheiro para leitura
	if (!f) //ficheiro inexistente
	{
		printf("O ficheiro deixou de existir!\n\n");
		return false;
	}
	printf("A carregar as imagens...\n\n");
	no_imagens *aux;
	(*imagens) = criar_lista_imagens(); //alocar espaco para as imagens
	while (!feof(f)) //enquanto a leitura nao chegar ao fim do ficheiro
	{
		aux = criar_imagem(); //alocar espaco para a imagem a ser lida
		fscanf(f, "%s %d %d %d", aux->nome, &aux->linhas, &aux->colunas, &aux->canais); //ler as caracteristicas da imagem
		if (feof(f)) //se chegou ao fim do ficheiro, a imagem nao e valida
		{
			free(aux); //eliminar o espaco alocado para a suposta ultima imagem
			return true;
		}
		aux->matriz_pixeis = criar_matriz_pixeis(aux->linhas, aux->colunas); //Alocar espaco para recriar a imagem numa matriz
		adicionar_imagem_lista(aux, (*imagens)); //colocar a imagem no final da lista de imagens
		
		//recriar imagem na matriz
		for (int linha = 0; linha < aux->linhas; linha++) //percorre as linhas
		{
			for (int coluna = 0; coluna < aux->colunas; coluna++) //percorre as colunas
			{
				//regista os dados do pixel e marca-o como nao verificado
				fscanf(f, "%d %d %d ", &aux->matriz_pixeis[linha][coluna].r, &aux->matriz_pixeis[linha][coluna].g, &aux->matriz_pixeis[linha][coluna].b);
				aux->matriz_pixeis[linha][coluna].verificado = false;
			}
		}
	}
	return true;
}

bool verificar_pixel(pixel_info *pixel, int r, int g, int b, int tolerancia)
{
	if (pixel->r < r - tolerancia || pixel->r > r + tolerancia) //cor vermelha invalida
		return false;
	if (pixel->g < g - tolerancia || pixel->g > g + tolerancia) //cor verde invalida
		return false;
	if (pixel->b < b - tolerancia || pixel->b > b + tolerancia) //cor azul invalida
		return false;
	return true;
}

void descobrir_blob(no_blob **blob, pixel_info *pixel, int linha, int coluna, int r, int g, int b, int tolerancia, no_imagens *imagem) //funcao recursiva
{
	if (linha < 0 || coluna < 0 || linha >= imagem->linhas || coluna >= imagem->colunas) //se coordenadas do pixel forem invalidas
		return;

	if (pixel->verificado == true) //se pixel ja foi analisado
		return;

	if (verificar_pixel(pixel, r, g, b, tolerancia) == false)
	{
		pixel->verificado = true;
		return;
	}

	pixel->verificado = true;
	no_pixeis *aux = criar_no_pixel(pixel, linha, coluna); //alocar espaco para guardar o pixel na lista de pixeis do blob
	if ((*blob) == NULL) //se o blob ainda nao foi criado
	{
		(*blob) = criar_blob(); //alocar espaco para o blob
		(*blob)->pixeis->inicio = aux;
	}
	else
	{
		aux->prox = (*blob)->pixeis->inicio;
		(*blob)->pixeis->inicio = aux;
	}
	(*blob)->pixeis->num_pixeis++; //incrementa o numero de pixeis do blob
	
	//preparar a determinacao da cor media do blob e da coordenadas do centro do blob
	(*blob)->media_r += pixel->r;
	(*blob)->media_g += pixel->g;
	(*blob)->media_b += pixel->b;
	(*blob)->coluna_centro += coluna;
	(*blob)->linha_centro += linha;

	descobrir_blob(blob, &imagem->matriz_pixeis[linha + 1][coluna], linha + 1, coluna, r, g, b, tolerancia, imagem); //verificar o pixel que esta por baixo
	descobrir_blob(blob, &imagem->matriz_pixeis[linha - 1][coluna], linha - 1, coluna, r, g, b, tolerancia, imagem); //verificar o pixel que esta por cima
	descobrir_blob(blob, &imagem->matriz_pixeis[linha][coluna + 1], linha, coluna + 1, r, g, b, tolerancia, imagem); //verificar o pixel que esta a direita
	descobrir_blob(blob, &imagem->matriz_pixeis[linha][coluna - 1], linha, coluna - 1, r, g, b, tolerancia, imagem); //verificar o pixel que esta a esquerda
}

void adicionar_blob_ordenado(no_blob *blob, lista_blobs **lista)
{
	no_blob *aux = (*lista)->inicio, *ant = NULL;

	if ((*lista)->inicio == NULL) //se a lista estiver vazia, adiciona-se o blob no inicio da lista
	{
		(*lista)->inicio = blob;
		(*lista)->num_blobs++; //incrementa o numero de blobs
		return;
	}

	while (aux)
	{
		if (aux->pixeis->num_pixeis > blob->pixeis->num_pixeis)
			break;

		ant = aux;
		aux = aux->prox;
	}

	if (ant != NULL) //se o ciclo nao parou no primeiro elemento
	{
		//colocar o blob a adicionar depois do auxiliar anterior
		ant->prox = blob;
		blob->prox = aux;
	}
	else
	{
		(*lista)->inicio = blob;
		blob->prox = aux;
	}
	(*lista)->num_blobs++;
}

void determinar_maior_distancia(no_imagens* imagem, max_min* max_e_min)
{
	if (imagem->blobs->inicio->distancia > max_e_min->maior_distancia)
	{
		max_e_min->maior_distancia = imagem->blobs->inicio->distancia;
		strcpy(max_e_min->imag_maior_distancia, imagem->nome);
	}
}

void determinar_distancia(no_imagens* imagem, no_blob *blob)
{
	no_blob *aux;
	aux = imagem->blobs->inicio;
	

	while (aux) {
		//determinar a distancia entre 2 blobs
		blob->distancia = sqrt(pow(blob->linha_centro - aux->linha_centro, 2) + (blob->coluna_centro - aux->coluna_centro, 2));
		aux = aux->prox;
	}

	
}

void determinar_max_min(no_imagens *imagem, max_min *max_e_min)
{
	no_blob *aux;
	aux = imagem->blobs->inicio;
	if (imagem->blobs->num_blobs > max_e_min->mais_blobs)
	{
		max_e_min->mais_blobs = imagem->blobs->num_blobs;
		strcpy(max_e_min->imag_mais_blobs, imagem->nome);
	}
	if (imagem->blobs)
	{
		while (aux)
		{
			if (aux->desvio_padrao < max_e_min->menor_desvio)
			{
				max_e_min->menor_desvio = aux->desvio_padrao;
				strcpy(max_e_min->imag_menor_desvio, imagem->nome);
				max_e_min->blob_infos = aux;
			}
			aux = aux->prox;
		}
	}
}

void determinar_desvio_padrao_blobs(no_blob *blob)
{
	no_pixeis *aux = blob->pixeis->inicio;

	//determinar a variancia das cores dos pixeis do blob
	while (aux)
	{
		blob->desvio_padrao_r += pow(aux->pixel->r - blob->media_r, 2);
		blob->desvio_padrao_g += pow(aux->pixel->g - blob->media_g, 2);
		blob->desvio_padrao_b += pow(aux->pixel->b - blob->media_b, 2);
		aux = aux->prox;
	}
	
	//determinar o desvio padrao das cores dos pixeis do blob
	blob->desvio_padrao_r = sqrtf(blob->desvio_padrao_r / blob->pixeis->num_pixeis);
	blob->desvio_padrao_g = sqrtf(blob->desvio_padrao_g / blob->pixeis->num_pixeis);
	blob->desvio_padrao_b = sqrtf(blob->desvio_padrao_b / blob->pixeis->num_pixeis);
	blob->desvio_padrao = (blob->desvio_padrao_r + blob->desvio_padrao_g + blob->desvio_padrao_b) / 3; //determinar o desvio padrao medio das 3 cores
}

void analisar_imagens(lista_imagens *imagens, int r, int g, int b, int tolerancia, max_min *max_e_min)
{
	no_imagens *imagem = imagens->inicio;
	no_blob *blob = NULL;
	no_blob* blob2 = NULL;

	while (imagem) //percorrer as imagens da lista
	{
		//percorrer a matriz com os pixeis da imagem
		for (int linha = 0; linha < imagem->linhas; linha++)
		{
			for (int coluna = 0; coluna < imagem->colunas; coluna++)
			{
				descobrir_blob(&blob, &imagem->matriz_pixeis[linha][coluna], linha, coluna, r, g, b, tolerancia, imagem);
				if (blob != NULL)
				{
					if (blob->pixeis->num_pixeis >= min_pix)
					{
						if (imagem->blobs == NULL)
						{
							imagem->blobs = criar_lista_blob();
						}
						
						//tirar as conclusoes finais acerca do blob
						blob->coluna_centro /= blob->pixeis->num_pixeis;
						blob->linha_centro /= blob->pixeis->num_pixeis;
						blob->media_r /= blob->pixeis->num_pixeis;
						blob->media_g /= blob->pixeis->num_pixeis;
						blob->media_b /= blob->pixeis->num_pixeis;
						determinar_desvio_padrao_blobs(blob); //determinar o desvio padrao do blob
						determinar_distancia(imagem, blob); //determinar a distancia entre blobs
			
						adicionar_blob_ordenado(blob, &imagem->blobs); //colocar o blob na lista de blobs da imagem, ordenados crescentemente
					}
					else
					{
						destruir_lista_pixeis_blob(blob->pixeis); //elimina o blob
						free(blob); 
					}

					blob = NULL;
				}
			}
		}
		determinar_max_min(imagem, max_e_min);
		determinar_maior_distancia(imagem, max_e_min);
		imagem = imagem->prox;
	}
}

void mostrar_resultados(lista_imagens *imagens, max_min *max_e_min)
{
	no_imagens *imagem = imagens->inicio;
	while (imagem)
	{
		printf("Imagem: %s\n", imagem->nome);
		if (imagem->blobs == NULL)
			printf("Tem 0 Blobs\n\n");
		else
			printf("Tem %d Blobs\n\n", imagem->blobs->num_blobs);
		imagem = imagem->prox;
	}

	printf("A imagem que possui mais Blobs e a %s com %d Blobs\n\n", max_e_min->imag_mais_blobs, max_e_min->mais_blobs);
	printf("A imagem que possui maior distancia entre clusters e a %s com %d de distancia\n\n", max_e_min->imag_maior_distancia, max_e_min->maior_distancia);
	
	if (max_e_min->blob_infos != NULL) //se foi possivel determinar a imagem com menor desvio padrao
	{
		printf("O blob que tem o menor desvio padrao pertence a imagem %s e tem as seguintes caracteristicas:\n", max_e_min->imag_menor_desvio);
		printf("  linha: %d\n  coluna: %d\n  Tem %d pixeis de tamanho\n  Tem %.3f de desvio padrao\n\n", max_e_min->blob_infos->linha_centro, max_e_min->blob_infos->coluna_centro, max_e_min->blob_infos->pixeis->num_pixeis, max_e_min->menor_desvio);
	}
	else
		printf("Nao foi possivel determinar o blob com menor desvio padrao\n\n");
}

void menu(int r, int g, int b, int tolerancia, char *nome_ficheiro)
{
	int opcao;
	lista_imagens *imagens = NULL;
	max_min *max_e_min = criar_max_min();
	do {
		printf("\n MENU\n");
		printf(" 1: Carregar ficheiro\n");
		printf(" 2: Analisar imagens\n");
		printf(" 3: Mostrar resultados\n\n");
		printf(" 0: Sair\n\n");
		printf(" Opcao: ");
		scanf("%d", &opcao);
		printf("\n");
		switch (opcao)
		{
		case 1:
			if (carregar_do_ficheiro(nome_ficheiro, &imagens) == false) //se o ficheiro nao foi carregado corretamente
			{
				free(max_e_min); 
				return;
			}
			break;

		case 2:
			analisar_imagens(imagens, r, g, b, tolerancia, max_e_min); //analisar imagem (determinar os blobs e tirar conclusoes finais)
			break;

		case 3:
			mostrar_resultados(imagens, max_e_min); //mostrar as conclusoes finais obtidas ao utilizador
			break;
		}
	} while (opcao != 0);
	
	//Eliminar os dados alocados
	destruir_lista_imagens(&imagens);
	free(max_e_min);
}

void all(int r, int g, int b, int tolerancia, char *nome_ficheiro)
{
	max_min *max_e_min = criar_max_min(); //alocar espaco para a variavel que contem os resultados das conclusoes finais
	lista_imagens *imagens = NULL;
	if (carregar_do_ficheiro(nome_ficheiro, &imagens) == false) //se o ficheiro nao foi carregado corretamente
	{
		free(max_e_min);
		return;
	}
	analisar_imagens(imagens, r, g, b, tolerancia, max_e_min); //analisar imagem (determinar os blobs e tirar conclusoes finais)
	mostrar_resultados(imagens, max_e_min); //mostrar as conclusoes finais obtidas ao utilizador
	
	//Eliminar os dados alocados
	destruir_lista_imagens(&imagens);
	free(max_e_min);
}

void mem(int r, int g, int b, int tolerancia, char *nome_ficheiro)
{
	while (1) //executa a funcao all de forma infinida de forma a verificar se a memoria e corretamente eliminada
	{
		all(r, g, b, tolerancia, nome_ficheiro);
	}
}