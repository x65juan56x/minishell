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
			if (errno == EINTR) // Interrupción por señal, reintentar
				continue;
			return (-1); // Error real
		}
		return (-1); // bytes_written == 0 es EOF inesperado
	}
}
