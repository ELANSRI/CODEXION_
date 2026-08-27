#include "codexion.h"

void	swap_dongles(t_dongle *dongle, int curr, int next)
{
	t_coder	*tmp;

	tmp = dongle->queue[curr];
	dongle->queue[curr] = dongle->queue[next];
	dongle->queue[next] = tmp;
}

void	heap_push(t_dongle *dongle, t_coder *coder)
{
	int		i;
	int		parent;
	long	v1, v2;

	i = dongle->size++;
	dongle->queue[i] = coder;
	while (i > 0)
	{
		parent = (i - 1) / 2;
		v1 = dongle->queue[i]->current_priority; // NOUVEAU
		v2 = dongle->queue[parent]->current_priority;
		if (v1 < v2)
		{
			swap_dongles(dongle, i, parent);
			i = parent;
		}
		else
			break ;
	}
}

int	is_smaller(t_dongle *dongle, int child, int smallest)
{
	long	child_dl;
	long	smallest_dl;

	if (child >= dongle->size)
		return (0);
	child_dl = dongle->queue[child]->current_priority;
	smallest_dl = dongle->queue[smallest]->current_priority;
	if (child_dl < smallest_dl)
		return (1);
	return (0);
}

int	get_smallest_child(t_dongle *dongle, int i)
{
	int	smallest;

	smallest = i;
	if (is_smaller(dongle, 2 * i + 1, smallest))
		smallest = 2 * i + 1;
	if (is_smaller(dongle, 2 * i + 2, smallest))
		smallest = 2 * i + 2;
	return (smallest);
}

void	heap_pop(t_dongle *dongle)
{
	int	i;
	int	smallest;

	if (dongle->size == 0)
		return ;
	dongle->queue[0] = dongle->queue[--dongle->size];
	i = 0;
	while (1)
	{
		smallest = get_smallest_child(dongle, i);
		if (smallest != i)
		{
			swap_dongles(dongle, i, smallest);
			i = smallest;
		}
		else
			break ;
	}
}