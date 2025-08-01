#include "libft.h"
#include <errno.h>

int	ft_putcharplus_fd(int fd, char c)
{
	ssize_t	bytes_written;

	while (1)
	{
		bytes_written = write(fd, &c, 1);
		if (bytes_written == 1)
			return (0);
		if (bytes_written == -1)
		{
			if (errno == EINTR)
				continue ;
			return (-1);
		}
		return (-1);
	}
}
