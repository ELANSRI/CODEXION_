#include "codexion.h"

int	can_take(t_coder *c, long now)
{
	if (c->left_dongle->in_use || now < c->left_dongle->available_at)
		return (0);
	if (c->right_dongle->in_use || now < c->right_dongle->available_at)
		return (0);
	return (1);
}

int	is_highest_priority(t_coder *c, long now)
{
	int		i;
	t_coder	*other;

	i = 0;
	while (i < c->data->number_of_coders)
	{
		other = &c->data->all_coders[i];
		if (other->id != c->id && other->state == 1)
		{
			if (other->left_dongle == c->left_dongle
				|| other->right_dongle == c->right_dongle
				|| other->left_dongle == c->right_dongle
				|| other->right_dongle == c->left_dongle)
			{
				if (other->current_priority < c->current_priority
					|| (other->current_priority == c->current_priority
						&& other->id < c->id))
					if (can_take(other, now))
						return (0);
			}
		}
		i++;
	}
	return (1);
}