#include "memory_manager.h"
#include <stdlib.h>
#include <stdio.h>


//=================================================================================
//	Cluster
//_________________________________________________________________________________

//�����������
Multi_Manager_45::Mini_Manager::Cluster::Cluster(int size, int elsize)
{
	cluster_size = size;
	elem_size = elsize;
	mas = (void*)malloc(cluster_size * elem_size);
	stack = (int*)malloc(cluster_size * sizeof(int));
	for (int i=0; i<cluster_size; i++) stack[i] = i;
	head = cluster_size;
	free_space = cluster_size;
	next_cluster = NULL;
	prev_cluster = NULL;
}

//����������
Multi_Manager_45::Mini_Manager::Cluster::~Cluster()
{
	free(mas);
	free(stack);
}

//��������� ������ ��� 1 �������
void* Multi_Manager_45::Mini_Manager::Cluster::get_memory()
{
	if (head == 0) return NULL; //������ ���������
	head--;
	free_space = head;
	//����� �� ����� ����� ���������� ��������, � ���������� ��������� �� ����, �� ������� mas
	return (void*)(int(mas) + stack[head]*elem_size);
}

//����������� �� ��������� ��������
bool Multi_Manager_45::Mini_Manager::Cluster::belong(void* element)
{
	int number; //����� �������� element � ������� mas
	number = (int(element) - int(mas)) / elem_size;
	if (number < 0 || number > cluster_size-1) return false; //������� �� � ����� ��������
	//���� ����� �������� ��������� � ����� ��������� ���������, �� �������� ����������� ��� ������
	for (int i=0; i<head; i++) if (stack[i] == number) return false;
	return true;
}

//������������ ������ ��� 1 �������
void Multi_Manager_45::Mini_Manager::Cluster::free_memory(void* element)
{
	int number; //����� �������� � ������� *mas
	if (head == cluster_size) return; //� �������� ��� ���������
	if (!belong(element)) return;
	number = (int(element) - int(mas)) / elem_size;
	stack[head++] = number;
	free_space = head;
}

//=================================================================================
//	Memory_Manager
//_________________________________________________________________________________

//�����������
Multi_Manager_45::Mini_Manager::Mini_Manager(int csize, int elsize)
{
	cluster_size = csize;
	elem_size = elsize;
	head = new Cluster(cluster_size, elem_size);
	head->next_cluster = NULL;
	tail = head;
	current = head;
	free_space = cluster_size;
}

//����������
Multi_Manager_45::Mini_Manager::~Mini_Manager()
{
	Cluster *next; //����� ������������ tail
	current = head;
	//������� ������ ���������
	while (current != NULL)
	{
		next = current->next_cluster;
		delete current;
		current = next;
	}
}

//��������� ������ ��� 1 �������
void* Multi_Manager_45::Mini_Manager::get_memory()
{
	void *result;
	//Cluster *temp;
	//�������� ������ �� �������� ��������
	result = current->get_memory();
	free_space--;
	//�������� ������� current ����������. � ����� ������
	//����� ����������� ��������� �� ��������� �������, ��� �������� �����.
	while (current->free_space == 0)
	{
		if (current->next_cluster == NULL)
		{
			tail = current->next_cluster = new Cluster(cluster_size, elem_size);
			current->next_cluster->prev_cluster = current;
			free_space += cluster_size;
		}
		current = current->next_cluster;
	}
	return result;
}

//������������ ������ ��� 1 �������
void Multi_Manager_45::Mini_Manager::free_memory(void* element)
{
	Cluster *temp, *found, *swap;
	//����������� ������ � �������� found
	found = head;
	while ((found != NULL) && !(found->belong(element))) found = found->next_cluster;
	if (found == NULL) return; //error, ������ �� � ���� ��������� ������
	found->free_memory(element);
	free_space++;
	//������ �� ����� ����� �����, � ������� ����� �� ������.
	temp = found;
	while (temp->next_cluster != NULL && temp->next_cluster->free_space < found->free_space)
		temp = temp->next_cluster;
	//������ ������� temp � found (����� 8 ������)
	//(!) �������� ������� ���� �� �������� ������� ������������ ���������, � �� ���������.
	if (temp != found)
	{
		//������ �������
		if (found != head && found->prev_cluster != temp) found->prev_cluster->next_cluster = temp;
		if (temp != head && temp->prev_cluster != found) temp->prev_cluster->next_cluster = found;
		if (found != tail && found->next_cluster != temp) found->next_cluster->prev_cluster = temp;
		if (temp != tail && temp->next_cluster != found) temp->next_cluster->prev_cluster = found;
		//������ �������
		//-----------
		if (temp->prev_cluster == found)
		{
			temp->prev_cluster = found->prev_cluster;
			found->prev_cluster = temp;
		} else
		if (found->prev_cluster == temp)
		{
			found->prev_cluster = temp->prev_cluster;
			temp->prev_cluster = found;
		} else
		{
		swap = temp->prev_cluster;
		temp->prev_cluster = found->prev_cluster;
		found->prev_cluster = swap;
		}
		//-----------
		if (temp->next_cluster == found)
		{
			temp->next_cluster = found->next_cluster;
			found->next_cluster = temp;
		} else
		if (found->next_cluster == temp)
		{
			found->next_cluster = temp->next_cluster;
			temp->next_cluster = found;
		} else
		{
		swap = temp->next_cluster;
		temp->next_cluster = found->next_cluster;
		found->next_cluster = swap;
		}
		//-----------
		//��� ������������� ������������ ������ � �����
		if (head == temp) head = found; else
		if (head == found) head = temp;
		if (tail == temp) tail = found; else
		if (tail == found) tail = temp;
		//
	}
	//����� ��������� �� ������ �� � ��� ������� ������� �.�.
	//�������� current ��� �� ��������� �� ������ �������� ��������� �������.
	if (current->prev_cluster != NULL && current->prev_cluster->free_space != 0) 
		current = current->prev_cluster;
	//����������� ��������, ���� �����
	if (tail->free_space == cluster_size && this->free_space == cluster_size*2/*!!!*/)
	{
		tail = tail->prev_cluster;
		delete tail->next_cluster;
		tail->next_cluster = NULL;
	}
}

//���������� ������ �������� ���������
int Multi_Manager_45::Mini_Manager::get_elem_size()
{
	return elem_size;
}

 //����������� �� �������� ������
bool Multi_Manager_45::Mini_Manager::is_empty()
{
	//���� ���� ����� 1 �������, � � ��� ������ �� ��������, �� �������� ������ ����
	return (head->next_cluster == NULL) && (head->free_space == cluster_size);
	//
}

//����������� �� ��������� ����� ��������� ������
bool Multi_Manager_45::Mini_Manager::belong(void* element)
{
	Cluster *found;
	found = head;
	//���� �������, �������� ����������� �������
	while ((found != NULL) && !(found->belong(element))) found = found->next_cluster;
	return (found != NULL);
}

//=================================================================================
//	Multi_Manager
//_________________________________________________________________________________


//�����������
Multi_Manager_45::Multi_Manager_45(int csize)
{
	//
	managers = NULL;
	cluster_size = csize;
	managers_count = 0;
	//
}

//����������
Multi_Manager_45::~Multi_Manager_45()
{
	//����������� ����-����������
	int i;
	for (i=0; i<managers_count; i++)
		delete managers[i];
	//
	if (managers_count > 0) free(managers);
	//
}

//��������� ������
void* Multi_Manager_45::get_memory(int size)
{
	//
	bool found;
	int i,n;
	//
	found = false;
	//���� ��������, ������� ������ �������� ������� size
	for (i=0; i<managers_count; i++)
		if (managers[i]->get_elem_size() == size) 
		{
			found = true;
			n = i;
		}
	//����� ��������, ������� ������ �������� ������ �������
	if (found) 
		return managers[n]->get_memory(); 
	else
	{ 	//���� �� �����, ������� ����� ��������
		managers_count++;
		if (managers == NULL)
		managers = (Mini_Manager**)malloc(managers_count * sizeof(Mini_Manager*)); else
		managers = (Mini_Manager**)realloc(managers,managers_count * sizeof(Mini_Manager*));
		managers[managers_count-1] = new Mini_Manager(cluster_size,size);
		return managers[managers_count-1]->get_memory();
	}
	//
}

//������������ ������
void Multi_Manager_45::free_memory(void* element)
{     
	int i,j;
	//����, ������ ��������� ������ ����������� ���� �������
	for (i=0; i<managers_count; i++)
		if (managers[i]->belong(element))
		{
			managers[i]->free_memory(element);
			if (managers[i]->is_empty()) //���� �������� ������ ������� - ������� ���
			{
				//
				delete managers[i];
				//
				for (j=i; j<managers_count-1; j++) 
					managers[j] = managers[j+1];
				//
				managers_count--;
				//
				if (managers_count == 0)
				{
					free(managers);
					managers = NULL;
				} else
					managers = (Mini_Manager**)realloc(managers,managers_count * sizeof(Mini_Manager*));
				//
			}
		}
	//
}

