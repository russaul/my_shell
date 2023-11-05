#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

enum
{
    buf = 8,
    symb = 0,
    ampersand = 2,
    out = 3,
    input = 4,
    input_end = 5,
    trait = 6,
    amper = 7,
};

struct list
{
    char *word;
    struct list *next;
};

struct check
{
    int Ampersand;
    int Out;
    int Input;
    int Input_end;
    int Trait;
};

char *array()
{
    int i = 1, k = 1, s, z;
    char *temp, *good;
    temp = malloc(buf);
    while (((s = getchar()) != '\n') && (s != EOF))
    {
        temp[i] = s;
        i++;
        if (i == k * buf)
        {
            k++;
            good = malloc(buf * (k + 1));
            for (z = 1; z < i; z++)
            {
                good[z] = temp[z];
            }
            free(temp);
            temp = good;
        }
    }
    temp[i] = 0;
    return temp;
}

int Compare(char *str)
{
    char mas[3] = {'c', 'd', 0};
    int i = 0;
    while (str[i] && mas[i] && str[i] == mas[i])
        i++;
    if (str[i] == 0 && mas[i] == 0)
        return 1;
    return 0;
}

int AnSymb(char *arg, int ind)
{
    int i = ind;
    char t;
    t = arg[i];
    switch (t)
    {
    case '&':
        return ampersand;
        break;
    case '>':
        return input;
        break;
    case '<':
        return out;
        break;
    case '|':
        return trait;
        break;
    default:
        return symb;
    }
}

void CheckSpecial(struct check *CheckList, int *special, char *arg, int *ind)
{
    int i = *ind, spec = *special;
    if (spec == ampersand)
        CheckList->Ampersand++;
    if (spec == out)
        CheckList->Out++;
    if (spec == input)
    {
        if (arg[i + 1] == '>')
        {
            CheckList->Input_end++;
            i++;
            spec = input_end;
            *special = spec;
            *ind = i;
        }
        else
        {
            CheckList->Input++;
        }
    }
    if (spec == trait)
        CheckList->Trait++;
}

char *NewWord(char *arg, int *error, int *ind,
              struct check *CheckList, int *special)
{
    char *temp = malloc(buf), *good;
    int i = *ind, er = *error, spec = 0, k = 1, z = 0, t = 0;
    while (((arg[i] != ' ') || (er == 1)) && (arg[i] != 0))
    {
        spec = AnSymb(arg, i);
        if ((spec == symb) || ((spec != symb) && (er == 1)))
        {
            if (t == buf * k)
            {
                k++;
                good = malloc(buf * (k + 1));
                for (z = 0; z < t; z++)
                    good[z] = temp[z];
                free(temp);
                temp = good;
            }
            if (arg[i] == '"')
            {
                er = !er;
                i++;
                continue;
            }
            temp[t] = arg[i];
        }
        if ((spec != symb) && (er == 0))
        {
            CheckSpecial(CheckList, &spec, arg, &i);
            *error = er;
            *ind = i;
            temp[t] = 0;
            *special = spec;
            return temp;
        }
        t++;
        i++;
    }
    temp[t] = 0;
    *ind = i;
    *error = er;
    return temp;
}

void WriteCheck(struct check *CheckList)
{
    printf("%d\n", CheckList->Ampersand);
    printf("%d\n", CheckList->Out);
    printf("%d\n", CheckList->Input);
    printf("%d\n", CheckList->Input_end);
}

int CheckAmp(const char *array, int ind, int *error)
{
    int i = ind + 1, er = *error;
    while (array[i] != 0)
    {
        if (array[i] != ' ')
        {
            *error = 1;
            return amper;
        }
        else
        {
            i++;
        }
    }
    *error = er;
    return 1;
}

int CheckBracket(struct check *CheckList, int *error)
{
    int er = *error;
    if ((CheckList->Out > 1) || (CheckList->Input > 1) ||
        (CheckList->Input_end > 1))
        return 1;
    if ((CheckList->Input == 1) && (CheckList->Input_end == 1))
        return 1;
    return er;
}

char *WriteBracket(int *spec)
{
    int s = *spec;
    char *mas = malloc(3);
    if (s == input)
    {
        mas[0] = '>';
        mas[1] = 0;
        *spec = 0;
    }
    if (s == input_end)
    {
        mas[0] = '>';
        mas[1] = '>';
        mas[2] = 0;
        *spec = 0;
    }
    if (s == out)
    {
        mas[0] = '<';
        mas[1] = 0;
        *spec = 0;
    }
    if (s == trait)
    {
        mas[0] = '|';
        mas[1] = 0;
        *spec = 0;
    }
    return mas;
}

void CheckingList2(struct list *TrueList, int *error)
{
    struct list *List = TrueList;
    while (List != NULL)
    {
        if (List->word[0] == '>')
        {
            if ((List->next == NULL) ||
                (List->next->word[0] == '<'))
            {
                *error = 1;
                return;
            }
            if ((List->next->next != NULL) &&
                (List->next->next->word[0] != '<'))
            {
                *error = 1;
                return;
            }
        }
        if (List->word[0] == '<')
        {
            if ((List->next == NULL) ||
                (List->next->word[0] == '>'))
            {
                *error = 1;
                return;
            }
            if ((List->next->next != NULL) &&
                (List->next->next->word[0] != '>'))
            {
                *error = 1;
                return;
            }
        }
        List = List->next;
    }
}

void CheckingList1(struct list *TrueList, int *error)
{
    struct list *List = TrueList;
    if ((List->word[0] == '>') || (List->word[0] == '<') ||
        (List->word[0] == '|'))
    {
        *error = 1;
        return;
    }
}

void CheckingList3(struct list *TrueList, int *error)
{
    struct list *List = TrueList;
    while (List != NULL)
    {
        if (List->word[0] == '|')
        {
            if ((List->next == NULL) ||
                (List->next->word[0] == '>') ||
                (List->next->word[0] == '<'))
            {
                *error = 1;
                return;
            }
        }
        List = List->next;
    }
}

void CheckingList4(struct list *TrueList, int *error)
{
    struct list *List = TrueList;
    int inout = 0;
    while (List != NULL)
    {
        if ((List->word[0] == '|') && (List->next == NULL))
        {
            *error = 1;
            return;
        }
        if ((List->word[0] == '>') || (List->word[0] == '<'))
        {
            inout = 1;
            List = List->next;
            continue;
        }
        if (((List->word[0] == '|') && (List->next->word[0] == '|')) ||
            ((List->word[0] == '|') && (inout == 1)))
        {
            *error = 1;
            return;
        }
        List = List->next;
    }
}

void CheckingList(struct list *TrueList, int *error)
{
    CheckingList1(TrueList, error);
    CheckingList2(TrueList, error);
    CheckingList3(TrueList, error);
    CheckingList4(TrueList, error);
}

struct list *Analyze(char *arg, int *err, struct check *CheckList, int *count)
{
    int i = 0, con = 0, er = 0, spec = 0;
    char *work;
    struct list *WorkList = NULL, *GodList = NULL, *Last = NULL;
    while (arg[i] != 0)
    {
        if (arg[i] != ' ')
        {
            if (er == 1)
            {
                break;
            }
            er = CheckBracket(CheckList, &er);
            if (CheckList->Ampersand == 1)
            {
                CheckList->Ampersand = CheckAmp(arg, i, &er);
                i++;
                continue;
            }
            if ((spec == input) || (spec == out) ||
                (spec == input_end) || (spec == trait))
            {
                work = WriteBracket(&spec);
                i++;
            }
            else
            {
                work = NewWord(arg, &er, &i, CheckList, &spec);
                if (work[0] == 0)
                {
                    free(work);
                    continue;
                }
            }
            WorkList = malloc(sizeof(struct list));
            WorkList->word = work;
            WorkList->next = NULL;
            if (Last)
            {
                Last->next = WorkList;
                Last = Last->next;
            }
            else
            {
                GodList = Last = WorkList;
            }
            con++;
        }
        if (arg[i] == ' ')
            i++;
    }
    er = CheckBracket(CheckList, &er);
    free(arg);
    CheckingList(GodList, &er);
    if ((Compare(GodList->word)) && (GodList->next->next != NULL))
        er = 1;
    *count = con;
    *err = er;
    return GodList;
}

void WriteMas(char **mas)
{
    int i = 0;
    while (mas[i] != 0)
    {
        printf("%s\n", mas[i]);
        i++;
    }
}

void WriteMasMas(char ***mas, int len)
{
    int i = 0;
    for (i = 0; i <= len; i++)
    {
        WriteMas(mas[i]);
        printf("\n");
    }
}

char **ListToAr(const struct list *WorkList, const int *count)
{
    int i = 0;
    char **ar = malloc((count[0] + 1) * sizeof(*ar));
    while (WorkList)
    {
        ar[i] = WorkList->word;
        WorkList = WorkList->next;
        i++;
    }
    ar[i] = NULL;
    return ar;
}

char ***ArToAr(char **mas, const int *count, int *leng)
{
    int i = 0, k = 0, ind = 0, fin = 0;
    char ***ar = malloc((count[0] + 1) * sizeof(**ar));
    while (mas[i] != NULL)
    {
        if (mas[i][0] != '|')
        {
            k++;
            i++;
            continue;
        }
        char **work = malloc((k + 1) * sizeof(*work));
        k = 0;
        while (mas[ind][0] != '|')
        {
            work[k] = mas[ind];
            k++;
            ind++;
        }
        work[k] = NULL;
        i++;
        ind++;
        k = 0;
        ar[fin] = work;
        fin++;
    }
    char **work = malloc((k + 1) * sizeof(*work));
    k = 0;
    while (mas[ind] != NULL)
    {
        work[k] = mas[ind];
        k++;
        ind++;
    }
    work[k] = NULL;
    ar[fin] = work;
    fin++;
    ar[fin] = NULL;
    *leng = fin - 1;
    printf("%d\n", *leng);
    free(mas);
    return ar;
}

void CleanZombie()
{
    while (wait4(-1, NULL, WNOHANG, NULL) > 0)
    {
    }
}

void NullCheck(struct check *CheckList)
{
    CheckList->Ampersand = 0;
    CheckList->Out = 0;
    CheckList->Input = 0;
    CheckList->Input_end = 0;
    CheckList->Trait = 0;
}

void OpenWrite(int flag, const char *word)
{
    int fd = 1;
    if (word != NULL)
    {
        fflush(stdout);
        if (flag == 2)
            fd = open(word, O_CREAT | O_WRONLY | O_APPEND, 0666);
        if (flag == 1)
            fd = open(word, O_CREAT | O_WRONLY | O_TRUNC, 0666);
        if (fd == -1)
        {
            perror(word);
            exit(1);
        }
    }
    dup2(fd, 1);
    close(fd);
}

void OpenRead(const char *word)
{
    int fd = 0;
    if (word != NULL)
    {
        fd = open(word, O_RDONLY, 0666);
        if (fd == -1)
        {
            perror(word);
            exit(1);
        }
    }
    dup2(fd, 0);
    close(fd);
}

char *Output(char **mas, int *fl)
{
    int i = 0;
    while (mas[i] != NULL)
    {
        if (mas[i][0] == '>')
        {
            if (mas[i][1] == '>')
            {
                *fl = 1;
                return mas[i + 1];
            }
            else
            {
                *fl = 2;
                return mas[i + 1];
            }
        }
        i++;
    }
    return NULL;
}

char *Input(char **mas)
{
    int i = 0;
    while (mas[i] != NULL)
    {
        if (mas[i][0] == '<')
            return mas[i + 1];
        i++;
    }
    return NULL;
}

void CleanOther(char **mas)
{
    int i = 0;
    while (mas[i] != NULL)
    {
        if ((mas[i][0] == '>') || (mas[i][0] == '<'))
        {
            while (mas[i] != NULL)
            {
                mas[i] = NULL;
                i++;
            }
            break;
        }
        i++;
    }
}

void Process(char **mas)
{
    execvp(mas[0], mas);
    perror(mas[0]);
    exit(1);
}

void CheckPid(int pid)
{
    if (pid == -1)
    {
        perror("fork: \n");
        exit(1);
    }
}

void First(int *fd, char **mas)
{
    close(fd[0]);
    dup2(fd[1], 1);
    close(fd[1]);
    Process(mas);
}

void Current(int *fd, char **mas, int old)
{
    dup2(old, 0);
    close(old);
    close(fd[0]);
    dup2(fd[1], 1);
    close(fd[1]);
    Process(mas);
}

void Last(int old, char **mas)
{
    dup2(old, 0);
    close(old);
    Process(mas);
}

void BigWait(int *pid, int len)
{
    int i;
    for (i = 0; i <= len; i++)
    {
        wait(NULL);
    }
}

void OneCommand(char ***array,
                struct check *CheckList,
                const char *in,
                const char *out,
                int flag)
{
    int pid, t = 0;
    if (Compare(array[0][0]))
    {
        t = chdir(array[0][1]);
        if (t == -1)
            perror(array[0][1]);
        free(array);
    }
    else
    {
        pid = fork();
        CheckPid(pid);
        if (pid == 0)
        {
            if (in != NULL)
                OpenRead(in);
            if (out != NULL)
                OpenWrite(flag, out);
            Process(array[0]);
        }
        if (CheckList->Ampersand == 0)
        {
            while (wait(NULL) != pid)
            {
            };
        }
        free(array);
    }
    CleanZombie();
}

void Command(char ***array, struct check *CheckList, int *len)
{
    int *pid, t = 0, l = *len, fd1[2], flag = 0, old;
    char *input = NULL, *output = NULL;
    pid = malloc(sizeof(int) * (l + 1));
    input = Input(array[l]);
    output = Output(array[l], &flag);
    CleanOther(array[l]);
    if (l == 0)
    {
        OneCommand(array, CheckList, input, output, flag);
    }
    else
    {
        pipe(fd1);
        old = fd1[0];
        pid[0] = fork();
        CheckPid(pid[0]);
        if (pid[0] == 0)
        {
            if (input != NULL)
                OpenRead(input);
            First(fd1, array[0]);
        }
        close(fd1[1]);
        for (t = 1; t < l; t++)
        {
            pipe(fd1);
            pid[t] = fork();
            CheckPid(pid[t]);
            if (pid[t] == 0)
                Current(fd1, array[t], old);
            close(old);
            close(fd1[1]);
            old = fd1[0];
        }
        pid[l] = fork();
        CheckPid(pid[l]);
        if (output != NULL)
            OpenWrite(flag, output);
        CleanOther(array[l]);
        if (pid[l] == 0)
            Last(old, array[l]);
        close(old);
        if (CheckList->Ampersand == 0)
            while ((wait(NULL)) != pid[l])
            {
            }
    }
    CleanZombie();
}

void WriteList(const struct list *WorkList)
{
    while (WorkList != NULL)
    {
        if (WorkList->word != NULL)
        {
            printf("[%s]\n", WorkList->word);
        }
        WorkList = WorkList->next;
    }
}

void DelList(struct list *WorkList)
{
    if (WorkList)
    {
        DelList(WorkList->next);
        free(WorkList->word);
        free(WorkList);
    }
}

int main()
{
    char *mas, **revers1, ***revers2;
    struct list *TrueList;
    struct check *CheckList = malloc(sizeof(struct check));
    int s = 0, er = 0, count = 0, leng = 0;
    printf("Hello, friend: ");
    while ((s = getchar()) != EOF)
    {
        if (s == '\n')
        {
            printf("Hello, friend: ");
            continue;
        }
        mas = array();
        mas[0] = s;
        if ((mas[0] == 0) || (mas[0] == '\n'))
        {
            free(mas);
            continue;
        }
        TrueList = Analyze(mas, &er, CheckList, &count);
        if (CheckList->Ampersand == amper)
            er = 1;
        if ((er == 1) || (TrueList == NULL))
        {
            DelList(TrueList);
            printf("Invalid input!\n");
            er = 0;
            NullCheck(CheckList);
            printf("Hello, friend: ");
            continue;
        }
        revers1 = ListToAr(TrueList, &count);
        revers2 = ArToAr(revers1, &count, &leng);
        Command(revers2, CheckList, &leng);
        NullCheck(CheckList);
        if (TrueList)
            DelList(TrueList);
        printf("Hello, friend: ");
    }
    printf("\n");
    free(CheckList);
    return 0;
}