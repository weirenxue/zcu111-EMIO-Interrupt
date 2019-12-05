# ZCU111 EMIO作為輸入並觸發的範例。
##### 開發工具：Vivado2019.1，SDK2019.1，ZCU111開發板與相關資料。

#### 1. 描述目標功能：指定兩個EMIO接到實體的pmod，一個為輸入一個為輸出，將兩個Pin用杜邦線連接在一起，並在c語言內設定輸入的中斷為正緣觸發，再控制輸出端的電壓，驗證輸入的正緣觸發是否正常。

#### 2. PL設計的部份，都與[zcu111-EMIO](https://github.com/weirenxue/zcu111-EMIO)範例相同，在此不再贅述。

#### 3. 將pmod的(J48.1)與(J49.1)用杜邦線接在一起。
<p align=center><img src="https://github.com/weirenxue/zcu111-EMIO-Interrupt/blob/readme-edits/pic/pmodConnect.png"  title="Pmod Connect" width="20%"></p>

#### 4. 程式設計概念：
        1. J49.1當作輸出，J48.1當作輸入。
        2. 中斷有三種東西要開啟
          a. Xil_Exception：用來處理例外事件。
          b. XScuGic：用來處理中斷事件。
          c. EMIO：EMIO自己也有中斷相關的API需要設定。
        3. 創建一個函式，裡面包含中斷發生後所需執行的指令，例如此範例：若進到中斷處理函數內，則print到函數內部。
        
#### 5. 燒錄後測試方式：使用SDK Terminal與ZCU111連線，並輸入0或1代表輸出的EMIO為高電位或低電位，觀察並發現只有當0轉為1時發生中斷。
          
