#pragma once
#include <stdlib.h>
#include <stdio.h>

//******************************************
//менеджер памяти, которым будет пользоваться контейнер.
//умеет хранить элементы любого размера.
class Multi_Manager_45
{
//private:
public:
	//******************************************
	//мини-менеджер памяти.
	//умеет хранить только элементы фисированного размера.
	class Mini_Manager
	{
	private:
		//******************************************
		//элемент двусвязного списка кластеров
		class Cluster
		{
		private:
			int elem_size;
			void *mas; //массив, который выделяется malloc-ом
			int *stack, head; //стек номеров свободных ячеек, такого же размера как *mas
		public:
			Cluster(int size, int elsize); //параметры: размер кластера, размер элемента
			~Cluster();
			int cluster_size, free_space; //количество всех ячеек, количество свободных ячеек
			void* get_memory(void);
			void free_memory(void*);
			bool belong(void*); //принадлежит ли указатель кластеру
			Cluster *next_cluster, *prev_cluster; //указатели на следующий, предыдущий кластер
		};
		//******************************************
		Cluster *head, *current, *tail; //первый кластер в списке, заполняющийся кластер, последний кластер
		int free_space; //количество пустых ячеек во всех кластерах
		int elem_size, cluster_size; //размер элементов в этом менеджере, кластеров
		//
	public:
		//
		int get_elem_size();
		bool is_empty(); //освободился ли менеджер памяти
		bool belong(void*); //принадлежит ли указатель менеджеру памяти
		//
		Mini_Manager(int csize, int elsize);//параметры: размер кластера, размер элемента
		~Mini_Manager();
		//
		void* get_memory();
		void free_memory(void*);
		//
	};
	//******************************************
	int cluster_size;
	Mini_Manager **managers;
	int managers_count;
	//
public:
	//изменяя размер кластера можно регулировать быстродействие/экономность
	Multi_Manager_45(int csize=65535);
	~Multi_Manager_45();
	//
	void* get_memory(int size);
	void free_memory(void*);
	//
};
//******************************************
