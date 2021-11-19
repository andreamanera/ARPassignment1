gcc command.c -o command
gcc m_x.c -o m_x 
gcc m_z.c -o m_z 
gcc inspection.c -o inspection
./m_x
./m_z
konsole -e ./command &
konsole -e ./inspection & 


