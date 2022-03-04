#include <QCoreApplication>
#include <QLocale>
#include <QTranslator>
#include <QtCore>

#define ERROR_NO_FILE_FOUND_MESSAGE "Could not find such file"
#define ERROR_COULD_NOT_OPEN_FILE_MESSAGE "Could not open file"
#define ERROR_NO_FILE_FOUND_ID 1
#define endl '\n'
#define dashsep "--------------------------------------------------------------------------------\n"

using namespace std;

QTextStream out(stdout);

void printTask(QVector<QVector<double>> task, QVector<QVector<int>> marks){
    for( int i = 0; i < task.count(); i++ ){
        for( int j = 0; j < task.at(0).count(); j++ ){
            if( task.at(i).at(j) < 10)
                out << " ";

            if( marks.at(i).at(j) != 0 ) out << "-";
            else out << " ";

            out << QString::number(abs(task.at(i).at(j)), 'f', 0);

            if( marks.at(i).at(j) != 0 ) out << "- ";
            else out << " ";
        }

        out << endl;
    }
}

void clearMarks(QVector<QVector<double>> &task, QVector<QVector<int>> &marks){
    for( int i = 0; i < task.count(); i++ ){
        for( int j = 0; j < task.at(0).count(); j++ ){
            marks[i][j] = 0;
        }
    }
}

QVector<QPoint> getOptimalSolution(QVector<QVector<double>> &task, QVector<QVector<int>> &marks){
    clearMarks(task, marks);

    QVector<QPoint> solution; solution.reserve(task.count());

    while( solution.count() != task.count() ){

    QVector<QPoint> potentialZeros; potentialZeros.reserve(task.count() * task.count());

    for( int i = 0; i < task.count(); i++ ){
        for( int j = 0; j < task.at(0).count(); j++ ){
            if( task.at(i).at(j) == 0 && marks.at(i).at(j) == 0 )
                potentialZeros.append(QPoint(i,j));
        }
    }

    QPoint bestCandidate(-1, -1);
    int minZeros = INT_MAX;
    for( QPoint p : potentialZeros ){
        if ( marks.at(p.x()).at(p.y()) != 0) continue;

        int zeros = 0;
        for( int i = 0; i < task.count(); i++ ){
            if ( task.at(i).at(p.y()) == 0 && marks.at(i).at(p.y()) == 0)
                zeros++;
        }
        for( int j = 0; j < task.count(); j++ ){
            if ( task.at(p.x()).at(j) == 0 && marks.at(p.x()).at(j) == 0)
                zeros++;
        }

        if(minZeros > zeros && zeros != 0){
            minZeros = zeros;
            bestCandidate = p;
        }
    }

    solution.append(bestCandidate);

    for( int i = 0; i < task.count(); i++ )
        marks[i][bestCandidate.y()] = 1;
    for( int j = 0; j < task.count(); j++ )
        marks[bestCandidate.x()][j] = 1;
    }

    /*while( solution.count() != task.count() ){
        QPoint cellCoord(-1, -1);
        int minZeros = INT_MAX;

        for( int i = 0; i < task.count(); i++ ){
            int zeros = 0;
            for( int j = 0; j < task.at(0).count(); j++ ){
                if( task.at(i).at(j) == 0 && marks.at(i).at(j) == 0)
                    zeros++;
            }

            if( minZeros < zeros && zeros != 0){
                minZeros = zeros;
                cellCoord = QPoint(i,j);
            }
        }

        for( int j = 0; j < task.count(); j++ ){
            int zeros = 0;
            for( int i = 0; i < task.at(0).count(); i++ ){
                if( task.at(i).at(j) == 0 && marks.at(i).at(j) == 0)
                    zeros++;
            }

            if( minZeros <= zeros && zeros != 0){
                minZeros = zeros;
                lineIndex = j;
            }
        }

        solution.append(QPair())

        for(int i = 0; i < task.at(0).count(); i++)
            marks[lineIndex][i] = 1;
        for(int i = 0; i < task.count(); i++)
            marks[i][lineIndex] = 1;
    }*/
    return solution;
}

bool CheckForMarkAvailability(QVector<QVector<double>> &task, QVector<QVector<int>> &marks){
    for( int i = 0; i < task.count(); i++ ){
        for( int j = 0; j < task.at(0).count(); j++ ){
            if( task.at(i).at(j) == 0 && marks.at(i).at(j) == 0 )
                return true;
        }
    }

    return false;
}


int MarkLine(QVector<QVector<double>> &task, QVector<QVector<int>> &marks){
    if( !CheckForMarkAvailability(task, marks) )
        return 0;

    int lineIndex = -1;
    int maxZeros = 0;
    bool horizontal = false;

    for( int i = 0; i < task.count(); i++ ){
        int zeros = 0;
        for( int j = 0; j < task.at(0).count(); j++ ){
            if( task.at(i).at(j) == 0 && marks.at(i).at(j) == 0)
                zeros++;
        }

        if( maxZeros < zeros ){
            maxZeros = zeros;
            lineIndex = i;
            horizontal = true;
        }
    }

    for( int j = 0; j < task.count(); j++ ){
        int zeros = 0;
        for( int i = 0; i < task.at(0).count(); i++ ){
            if( task.at(i).at(j) == 0 && marks.at(i).at(j) == 0)
                zeros++;
        }

        if( maxZeros <= zeros ){
            maxZeros = zeros;
            lineIndex = j;
            horizontal = false;
        }
    }

    if( horizontal ){
        for(int i = 0; i < task.at(0).count(); i++)
            marks[lineIndex][i]++;
    } else {
        for(int i = 0; i < task.count(); i++)
            marks[i][lineIndex]++;
    }

    return MarkLine(task, marks) + 1;
}

QVector<QPoint> solveTask(QVector<QVector<double>> &task, QVector<QVector<int>> &marks){
    out << "Reducing matrix" << endl;
    out << dashsep;
    printTask(task, marks);
    out << dashsep << endl;

    for( int i = 0; i < task.count(); i++ ){
        double minInLine = task.at(i).at(0);
        for( int j = 1; j < task.at(0).count(); j++ ){
            if( minInLine > task.at(i).at(j))
                minInLine = task.at(i).at(j);
        }

        for( int j = 0; j < task.at(0).count(); j++ ){
            task[i][j] -= minInLine;
        }
    }

    for( int j = 0; j < task.at(0).count(); j++ ){
        double minInLine = task.at(0).at(j);
        for( int i = 1; i < task.count(); i++ ){
            if( minInLine > task.at(i).at(j))
                minInLine = task.at(i).at(j);
        }

        for( int i = 0; i < task.at(0).count(); i++ ){
            task[i][j] -= minInLine;
        }
    }

    out << "Reduced matrix" << endl;
    out << dashsep;
    printTask(task, marks);
    out << dashsep << endl;

    int markedLines = MarkLine(task, marks);
    out << "Marked matrix" << endl;
    out << dashsep;
    printTask(task, marks);
    out << dashsep << endl;

    out << "Number of mark lines is " << markedLines << " and task size is " << task.count() << endl;
    if( markedLines >= task.count()){
        out << "Reached optimal solution" << endl;
        return getOptimalSolution(task, marks);
    }

    double minInTask = -1;
    for( int i = 0; i < task.count(); i++ ){
        for( int j = 0; j < task.at(0).count(); j++ ){
            if( marks.at(i).at(j) == 0 && (task.at(i).at(j) < minInTask || minInTask == -1))
                minInTask = task.at(i).at(j);
        }
    }

    for( int i = 0; i < task.count(); i++ ){
        for( int j = 0; j < task.at(0).count(); j++ ){
            if( marks.at(i).at(j) == 0 )
                task[i][j] -= minInTask;
            else if( marks.at(i).at(j) == 2)
                task[i][j] += minInTask;
        }
    }

    return solveTask(task, marks);
}

bool readTaskFromFile(QString filePath, char sep,  QVector<QVector<double>> &task, QVector<QVector<int>> &marks){
    QFile taskFile(filePath, nullptr);

    if( !taskFile.exists() ) {
        out << ERROR_NO_FILE_FOUND_MESSAGE;
        return false;
    }

    if( !taskFile.open(QIODevice::ReadOnly) ){
        out << ERROR_COULD_NOT_OPEN_FILE_MESSAGE;
        return false;
    }

    int n = taskFile.readLine().split(sep).count();
    int m = 1;

    while( !taskFile.atEnd() ){
        taskFile.readLine();
        m++;
    }

    taskFile.seek(0);

    task = QVector<QVector<double>>(m);
    marks = QVector<QVector<int>>(m);

    for( int i = 0; i < m; i++ ){
        task[i] = QVector<double>(n);
        marks[i] = QVector<int>(n);

        QList<QByteArray> line = taskFile.readLine().split(sep);
        for( int j = 0; j < n; j++ ){
            task[i][j] = line.at(j).toInt();
        }
    }
}

int main(int argc, char *argv[])
{
    QVector<QVector<double>> task;
    QVector<QVector<double>> initialTask;
    QVector<QVector<int>> marks;

#ifndef QT_DEBUG
    if( !readTaskFromFile(QString(argv[1]), *argv[2], task, marks) ) return 1;
#else
    if( !readTaskFromFile(QDir::currentPath() + "/debug/matrix", ' ', task, marks) ) return 1;
#endif

    initialTask = task;

    QVector<QPoint> solution = solveTask(task, marks);

    double sum = 0;
    for (QPoint p : solution){
        sum += initialTask.at(p.x()).at(p.y());
        out << "+" << initialTask.at(p.x()).at(p.y());
    }
    out << " = " << sum << endl;

    return 0;
}


