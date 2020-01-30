void parent(int const fd[])
{
    close(fd[0]);
    close(fd[1]);

    for(int i=0; i<2; ++i) wait(0);
}

void child1(int const fd[])
{
    close(fd[0]); // libère la lecture
    dup2(fd[1], 1); // libère l'écriture
    execlp("date", "", NULL);
}

void child2(int const fd[])
{
    close(fd[1]); // libère l'écriture
    dup2(fd[0], 0); // redirige la lecture
    execlp("tr", "", "[:lower:]", "[:upper:]", NULL);
}

int main()
{
    int fd[2];
    pipe(fd);
    if(!fork()) child1(fd);
    else if(!fork()) child2(fd);
    else parent(fd);
}

// 