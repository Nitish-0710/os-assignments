db="student.txt"

while true
do
    echo "----------------------------"
    echo "      Student Database"
    echo "----------------------------"
    echo "1. Create Database"
    echo "2. View Database"
    echo "3. Insert Record"
    echo "4. Delete Record"
    echo "5. Modify Record"
    echo "6. Display Result"
    echo "7. Exit"
    echo "----------------------------"

    read -p "Enter your choice: " ch

    case $ch in

    1)
        > "$db"
        echo "Database Created Successfully."
        ;;

    2)
        if [ -f "$db" ]
        then
            echo "Roll   Name   Marks"
            echo "--------------------"
            cat "$db"
        else
            echo "Database does not exist."
        fi
        ;;

    3)
        read -p "Enter Roll Number: " roll
        read -p "Enter Name: " name
        read -p "Enter Marks: " marks

        echo "$roll $name $marks" >> "$db"

        echo "Record Inserted Successfully."
        ;;

    4)
        read -p "Enter Roll Number to Delete: " roll

        if [ -f "$db" ]
        then
            awk -v r="$roll" '$1 != r' "$db" > temp.txt
            mv temp.txt "$db"

            echo "Record Deleted Successfully."
        else
            echo "Database does not exist."
        fi
        ;;

    5)
        read -p "Enter Roll Number to Modify: " roll
        read -p "Enter New Name: " name
        read -p "Enter New Marks: " marks

        if [ -f "$db" ]
        then
            awk -v r="$roll" -v n="$name" -v m="$marks" '
            {
                if ($1 == r)
                    print r, n, m
                else
                    print
            }' "$db" > temp.txt

            mv temp.txt "$db"

            echo "Record Modified Successfully."
        else
            echo "Database does not exist."
        fi
        ;;

    6)
        read -p "Enter Roll Number: " roll

        if [ -f "$db" ]
        then
            awk -v r="$roll" '
            $1 == r {
                print "Roll Number :", $1
                print "Name         :", $2
                print "Marks        :", $3

                if ($3 >= 40)
                    print "Result       : Pass"
                else
                    print "Result       : Fail"
            }' "$db"
        else
            echo "Database does not exist."
        fi
        ;;

    7)
        echo "Thank You."
        exit
        ;;

    *)
        echo "Invalid Choice."
        ;;

    esac

    echo
done
