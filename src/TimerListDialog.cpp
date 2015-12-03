/*
 * TimerListDialog.cpp is part of Brewtarget, and is Copyright the following
 * authors 2009-2014
 * - Aidan Roberts <aidanr67@gmail.com>
 *
 * Brewtarget is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Brewtarget is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "TimerListDialog.h"
#include "recipe.h"
#include "hop.h"
#include <QMessageBox>

TimerListDialog::TimerListDialog(MainWindow* parent) : QDialog(parent),
    mainWindow(parent)
{
   setupUi(this);  
   boilTime->setBoilTime(setBoilTimeBox->value() * 60); //default 60mins
   timers = new QList<TimerDialog*>();
   connect(boilTime, SIGNAL(BoilTimeChanged()), this, SLOT(decrementTimer()));
   connect(boilTime, SIGNAL(timesUp()), this, SLOT(timesUp()));
   updateTime();
   stopButton->setEnabled(false);
   resetButton->setEnabled(false);
   setInitialTimerPosition();
}

TimerListDialog::~TimerListDialog()
{
}

void TimerListDialog::setInitialTimerPosition()
{
    //Used to cascade timer dialogs- Better way to do this?
    timerPositions->clear();
    timerPositions->push(50);
    timerPositions->push(50);
}

void TimerListDialog::on_addTimerButton_clicked()
{
   TimerDialog* newTimer = new TimerDialog(this, boilTime);
   timers->append(newTimer);
   positionNewTimer(newTimer);
   newTimer->show();
}

void TimerListDialog::positionNewTimer(TimerDialog *t)
{
    int x = timerPositions->pop();
    int y = timerPositions->pop();
    t->move(x, y);
    timerPositions->push(x + 50);
    timerPositions->push(y + 50);
}

void TimerListDialog::on_startButton_clicked()
{
    boilTime->startTimer();
    stopButton->setEnabled(true);
    resetButton->setEnabled(true);

}

void TimerListDialog::on_stopButton_clicked()
{
    boilTime->stopTimer();
    stopButton->setEnabled(false);
    loadRecipesButton->setEnabled(true);
}

void TimerListDialog::on_resetButton_clicked()
{
    // Reset boil time to defined boil time
    boilTime->setBoilTime(setBoilTimeBox->value() * 60);
    updateTime();
    foreach (TimerDialog* t, *timers)
        t->reset();
    resetButton->setEnabled(false);
    loadRecipesButton->setEnabled(true);
}

void TimerListDialog::on_setBoilTimeBox_valueChanged(int t)
{
    boilTime->setBoilTime(t * 60);
    updateTime();
}

void TimerListDialog::decrementTimer()
{
    if(!resetButton->isEnabled())
        resetButton->setEnabled(true);
    updateTime();
}

void TimerListDialog::updateTime()
{
   unsigned int time = boilTime->getTime();
   timeLCD->display(timeToString(time));
}

QString TimerListDialog::timeToString(int t)
{
    unsigned int seconds = t;
    unsigned int minutes = 0;
    unsigned int hours = 0;
   if (t == 0)
       return "00:00:00";
   if (t > 59){
       seconds = t%60;
       minutes = t/60;
       if (minutes > 59){
          hours = minutes/60;
          minutes = minutes%60;
       }
   }
   QString secStr, minStr, hourStr;
   if (seconds < 10)
       secStr = "0" + QString::number(seconds);
   else
       secStr = QString::number(seconds);
   if (minutes <10)
       minStr = "0" + QString::number(minutes);
   else
       minStr = QString::number(minutes);
   if (hours < 10)
       hourStr = "0" + QString::number(hours);
   else
       hourStr = QString::number(hours);
   return hourStr + ":" + minStr + ":" + secStr;
}


void TimerListDialog::on_hideButton_clicked()
{
    foreach (TimerDialog* t, *timers) {
        if (!t->isHidden())
            t->hideTimer();
    }
}

void TimerListDialog::on_showButton_clicked()
{
    foreach (TimerDialog* t, *timers) {
        if (t->isHidden())
            t->showTimer();
    }
}

void TimerListDialog::timesUp()
{
    //Do something cool
}

void TimerListDialog::on_loadRecipesButton_clicked()
{
    //Load current recipes
    if (!timers->isEmpty()){
        QMessageBox mb;
        mb.setText("Active Timers");
        mb.setInformativeText("You currently have active timers, would you like to replace them or add to them?");
        QAbstractButton *replace =  mb.addButton(tr("Replace"), QMessageBox::YesRole);
        QAbstractButton *add = mb.addButton(tr("Add"), QMessageBox::NoRole);
        mb.setIcon(QMessageBox::Question);
        mb.exec();
        if (mb.clickedButton() == replace)
            removeAllTimers();
    }
    enum Use {Mash, First_Wort, Boil, UseAroma, Dry_Hop }; //For hop comparisons
    Use boil = Boil;
    Recipe* recipe = mainWindow->currentRecipe();
    bool timerFound = false;
    QList<Hop*> hops;
    QString note;
    hops = recipe->hops();
    foreach (Hop* h, hops) {
        if (h->use() == boil) {
            note = QString::number(int(h->amount_kg()*1000)) +
                    "g of " + h->name(); // TODO - show amount in brewtarget selected units
            int newTime = h->time_min() * 60;
            foreach (TimerDialog* td, *timers) {
                if (td->getTime() == newTime){
                    td->setNote(note); //append note to existing timer
                    timerFound = true;
                }
            }
            if (!timerFound) {
                TimerDialog * newTimer = new TimerDialog(this, boilTime);
                newTimer->setTime(h->time_min()*60);
                newTimer->setNote(note);
                timers->append(newTimer);
                positionNewTimer(newTimer);
                newTimer->show();
            }
        timerFound = false;
        }
    }
    loadRecipesButton->setEnabled(false);

}

void TimerListDialog::on_cancelButton_clicked()
{
    removeAllTimers();
}

void TimerListDialog::removeAllTimers()
{
    qDeleteAll(*timers);
    timers->clear();
    loadRecipesButton->setEnabled(true);
    setInitialTimerPosition();

}

void TimerListDialog::removeTimer(TimerDialog *t)
{
    //Return position to stack
    timerPositions->push(t->x());
    timerPositions->push(t->y());
    for (int i = 0; i < timers->count(); i++) {
        if (timers->at(i) == t) {
            delete(timers->at(i));
            timers->removeAt(i);
        }
    }
}

