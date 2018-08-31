

gdb-multiarch build/ch.elf <<END

target remote | openocd -f board/stm32f4discovery.cfg -c "gdb_port pipe; log_output openocd.log"

monitor reset init
monitor halt

monitor arm semihosting enable
symbol-file "./build/ch.elf"
load "./build/ch.elf"

monitor reset halt
monitor halt

monitor reset run

END


