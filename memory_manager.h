#pragma once
#include <stdlib.h>
#include <stdio.h>

//******************************************
//�������� ������, ������� ����� ������������ ���������.
//����� ������� �������� ������ �������.
class Multi_Manager_45
{
//private:
public:
	//******************************************
	//����-�������� ������.
	//����� ������� ������ �������� ������������� �������.
	class Mini_Manager
	{
	private:
		//******************************************
		//������� ����������� ������ ���������
		class Cluster
		{
		private:
			int elem_size;
			void *mas; //������, ������� ���������� malloc-��
			int *stack, head; //���� ������� ��������� �����, ������ �� ������� ��� *mas
		public:
			Cluster(int size, int elsize); //���������: ������ ��������, ������ ��������
			~Cluster();
			int cluster_size, free_space; //���������� ���� �����, ���������� ��������� �����
			void* get_memory(void);
			void free_memory(void*);
			bool belong(void*); //����������� �� ��������� ��������
			Cluster *next_cluster, *prev_cluster; //��������� �� ���������, ���������� �������
		};
		//******************************************
		Cluster *head, *current, *tail; //������ ������� � ������, ������������� �������, ��������� �������
		int free_space; //���������� ������ ����� �� ���� ���������
		int elem_size, cluster_size; //������ ��������� � ���� ���������, ���������
		//
	public:
		//
		int get_elem_size();
		bool is_empty(); //����������� �� �������� ������
		bool belong(void*); //����������� �� ��������� ��������� ������
		//
		Mini_Manager(int csize, int elsize);//���������: ������ ��������, ������ ��������
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
	//������� ������ �������� ����� ������������ ��������������/�����������
	Multi_Manager_45(int csize=65535);
	~Multi_Manager_45();
	//
	void* get_memory(int size);
	void free_memory(void*);
	//
};
//******************************************
