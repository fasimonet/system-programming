void parent(int const fd[])
{
    close(fd[0]);
    close(fd[1]);

    for(int i=0; i<2; ++i) wait(0);
}