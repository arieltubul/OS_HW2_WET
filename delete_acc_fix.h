
/*in Bank.h class Bank members public*/
void Delete_From_Map(int acc_id,Bank_Account *acc);



/*in Bank.cpp*/
void Bank::Delete_From_Map(int acc_id,Bank_Account *acc){
	map<int, Bank_Account*>::iterator it = _account_list.find(acc_id);
    acc = it->second;
	_account_list.erase(it);
	
}


/*in atm.cpp in void Atm::Close_account(int account, int password)*/
.
.
.
        if (Check_password(account, password)) {
            //password match
			Bank_Account *tmp;
            Bank_Account &curr = bank->get_account(account);
            //FIXME what happens when i delete this account sempahore ?
            // meybe hold the write semaphore ?
            curr.account_write_lock();
            Bank_Log->lock_log_file();
		    Bank_Log->_log << this->_atm_num << ": Account " << account << " is now closed. Balance was " << curr.get_balance() << endl;
		    Bank_Log->unlock_log_gile();
			bank->Delete_From_Map(account,tmp);
            tmp->account_write_unlock;
			delete tmp;
        }
.
.
.