
void *_os_stack_init(void (*func)(void *), void *param, void *stack, unsigned int stack_size)
{
    unsigned int *sp = (unsigned int*)((unsigned int)stack + stack_size);

    *(--sp) = (unsigned int)func;
    *(--sp) = (unsigned int)0x07070707;
    *(--sp) = (unsigned int)0x06060606;
    *(--sp) = (unsigned int)0x05050505;
    *(--sp) = (unsigned int)0x04040404;
    *(--sp) = (unsigned int)0x11111111;
    *(--sp) = (unsigned int)0x10101010;
    *(--sp) = (unsigned int)0x09090909;
    *(--sp) = (unsigned int)param;

    return sp;
}
