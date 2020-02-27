section .data
num_four: dq 4.0

section .text
global mandelbrot_x86
    ; function arguments:
    ; rdi - PixelBuffer, rsi - width, rdx - height, xmm0 - x0, xmm1 - y0, xmm2 - step
mandelbrot_x86:    
    push rbp
    mov rbp, rsp  ; function prologue
    
    mov rax, rsi  ; rax stores width, will use as x counter (need new register, counter starts from max for every line)
                  ; rdx stores height, will use as y counter

    movsd xmm9, [num_four]
    movsd xmm8, xmm0    ; min x
loop1:
    ; iterations counter - 0
    ; xmm3 - x
    ; xmm4 - y
    mov r9, 0x000000
    movsd xmm3, xmm0
    movsd xmm4, xmm1
loop2:
    ; xmm5 - x^2
    ; xmm6 - y^2
    ; xmm7 - x^2 + y^2
    movsd xmm5, xmm3
    mulsd xmm5, xmm5
    
    movsd xmm6, xmm4
    mulsd xmm6, xmm6
 
    movsd xmm7, xmm5
    addsd xmm7, xmm6

    comisd xmm7, xmm9
    jnb colour

    ; x_next = x^2 - y^2 + x0
    ; y_next = 2xy + y0
    movsd xmm7, xmm5
    subsd xmm7, xmm6
    addsd xmm7, xmm0
    
    movsd xmm5, xmm3
    mulsd xmm5, xmm4
    addsd xmm5, xmm5
    addsd xmm5, xmm1
    movsd xmm4, xmm5

    movsd xmm3, xmm7

    add r9, 0x010101

    cmp r9, 0xFFFFFF  ; max 255 iterations, the more iterations the whiter the pixel
    jne loop2
colour:
    mov [rdi], r9  
    add rdi, 4

    ; x - x + step
    addsd xmm0, xmm2
    dec rax
    jnz loop1  ; decrement x counter if not zero keep looping x

    ; xmm0 - min x
    ; xmm1 - xmm1 + step
    movsd xmm0, xmm8
    addsd xmm1, xmm2
    mov rax, rsi ; reset x counter
    dec rdx      ; update y counter
    jnz loop1    ; if not zero keep looping y

    mov rsp, rbp
    pop rbp      ; function epilogue
    ret