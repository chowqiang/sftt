#!/bin/bash
git status |grep "git add" -A 7 |tail -n 6|head -n 6 |awk '{print $2}' |xargs file
